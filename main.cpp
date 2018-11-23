// -*- C++ -*-
#include "common_types.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>  
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <random>
#include <chrono>
#include <thread>
#include <mutex>
#include <set>

class Random {
public: 
    Random(int m) { 
        rng.seed(std::random_device()()); 
        dist = new std::uniform_int_distribution<std::mt19937::result_type>(0,m);
    }
    int operator()() {
        return (*dist)(rng);
    }
private:
    std::mt19937 rng;
    std::uniform_int_distribution<std::mt19937::result_type> * dist;
};

class Timer {
    //==== Usage ====
    // StoreTimes t;
    // t.storeInitialTime();
    // Timer * timer = new Timer(1500, false, &StoreTimes::storeFinalTime, &t);

public:
    template <class callable, class... arguments>
        Timer(int after, bool async, callable&& f, arguments&&... args)
    {
        std::function<typename std::result_of<callable(arguments...)>::type()>
            task(std::bind(std::forward<callable>(f),
                           std::forward<arguments>(args)...));

        if (async) {
            std::thread([after, task]() {
                    std::this_thread::sleep_for(std::chrono::milliseconds(after));
                    task();
                }).detach();
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(after));
            task();
        }
    }
};

class WatchDog {
public:
    WatchDog(int per = 1000, int rdPipe = 0) 
        : msWdArmPeriod(per), readPipeEnd(rdPipe), armed(true) {}

    void start() { 
        Timer * timer = new Timer(msWdArmPeriod, true, &WatchDog::trigger, this);
        run();
    }

    void arm() { armed = true; }
    bool isArmed() { return armed; }

protected:
    void trigger() {
        if (! armed) {
            notArmedAction();
        } else {
            armed = false;
            Timer * timer = new Timer(msWdArmPeriod, true, &WatchDog::trigger, this);
        }
    }

    virtual void notArmedAction()=0;
    virtual void run()=0;   

protected:
    int msWdArmPeriod;
    int readPipeEnd;
    bool armed;
};

class SimpleWatchDog : public WatchDog {
public:
    SimpleWatchDog(int per = 1000, int rdPipe = 0) 
        : WatchDog(per, rdPipe), wdFile(std::string(mktemp(wdFileNameTpl))) {}

protected:
    virtual void notArmedAction() {
        // do whatever the watchdog must do if the parent failed to arm it
        std::cerr << "GUAU!\n";

        std::lock_guard<std::mutex> lock(setOfIDsMutex);
        setOfIDs.clear();
        exit(32);
    }

    virtual void run() {
        // This watchdog receives a set of commands via pipe
        // The commands accepted are:
        // add <id> : add the docker container <id> to the list of running containers
        // rm <id>  : remove the docker container <id> form the list of running containers
        // clear : clears the list of docker containers
        // asm : Re-arms the watchdog
        // quit : Quit the watchdog

        std::vector<std::string> msgList;
        char message[128];
        size_t pos;

        while (true) { // forever 
            memset(message, 0, sizeof(message));
            int len = read(readPipeEnd, message, 128);
            std::string msg(message);
            std::cerr << "Received: '" << msg << "'\n";
            // Split into single messages
            msgList.clear();
            while ((pos = msg.find(";")) != std::string::npos) {
                msgList.push_back(msg.substr(0, pos));
                msg.erase(0, pos + 1);
            }

            // Parse and execute messages
            for (auto & m : msgList) {
                std::cerr << "  Processing '" << m << "'\n";
                if (m == "quit") {
                    // Quit watchdog
                    std::cerr << "Quitting watchdog . . .\n";
                    return;
                } else if (m == "clear") {
                    // Re-arm watchdog
                    std::cerr << "Clearing list . . .\n";
                    std::lock_guard<std::mutex> lock(setOfIDsMutex);
                    setOfIDs.clear();
                } else if (m == "dump") {
                    // Re-arm watchdog
                    std::cerr << "List of IDs:\n";
                    std::lock_guard<std::mutex> lock(setOfIDsMutex);
                    for (auto & id : setOfIDs) {
                        std::cerr << "    - " << id << '\n';
                    }
                } else if (m == "arm") {
                    // Re-arm watchdog
                    arm();
                    std::cerr << "Re-arming watchdog\n";
                } else {
                    pos = m.find(" ");
                    std::string token = m.substr(0, pos);
                    m.erase(0, pos + 1);
                    if (token == "add") {
                        // Add <id> to list
                        std::lock_guard<std::mutex> lock(setOfIDsMutex);
                        setOfIDs.insert(m);
                    } else if (token == "rm") {
                        // Remove <id> form list
                        std::lock_guard<std::mutex> lock(setOfIDsMutex);
                        setOfIDs.erase(m);
                    } else {
                        std::cerr << "Unknown message: '" << message << "'\n";
                    }
                }
            }
        } // forever (until exited)
    }   

private:
    std::set<std::string> setOfIDs;
    std::mutex setOfIDsMutex;

    std::string wdFile;
    static char wdFileNameTpl[];
};

char SimpleWatchDog::wdFileNameTpl[] = "/tmp/wd.XXXXXX";

int createWatchDog(int lapse)
{
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid1;
    pid_t pid2;
    int status;
    if ((pid1 = fork())) {
        /* parent process A */
        waitpid(pid1, &status, 0);
        close(pipefd[0]);
        return pipefd[1];
    } else if (! pid1) {
        /* child process B */
        if ((pid2 = fork())) {
            exit(0);
        } else if (!pid2) {
            /* child process C */
            close(pipefd[1]);
            pid_t sid = setsid();
            SimpleWatchDog wd(lapse, pipefd[0]);
            wd.start();
            exit(0);
        } else {
            perror("fork(2)");
            exit(EXIT_FAILURE);
        }
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
}

//======================================================================
// Main function
//======================================================================
int main(int argc, char * argv[])
{
    int wdHdl = createWatchDog(2000);
    char message[128];
    int len;

    for (int i = 1; i < 1000; ++i) {
        std::cout << i << '\n';
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if ((i < 7) || (i >= 11)) {
            len = sprintf(message, "add 000-%i-000;", i);
            write(wdHdl, message, len);
            len = sprintf(message, "arm;");
            write(wdHdl, message, len);
        }
        if (i > 4) {
            len = sprintf(message, "rm 000-%i-000;", i - 3);
            write(wdHdl, message, len);
        }             
        if ((i % 6) == 0) {
            len = sprintf(message, "dump;");
            write(wdHdl, message, len);
        }
        if (i == 20) {
            len = sprintf(message, "quit;");
            write(wdHdl, message, len);
        }
    }


    return 0;
}
