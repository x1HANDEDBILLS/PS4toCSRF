#include <QApplication>
#include <QSocketNotifier>
#include "windows/mainwindow.h"
#include "theme/theme.h"
#include "logic/logic.h"
#include "input/input_manage.h"
#include "profiles/profile_manager.h"
#include "common_constants.h"
#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <atomic>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <time.h>

std::atomic<bool> keep_running(true);
int sig_pipe[2];

void signalHandler(int signum) {
    char a = 1;
    if (write(sig_pipe[1], &a, 1) == -1) { }
}

void* logic_thread_func(void* arg) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(ISOLATED_CORE, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    
    struct sched_param param; 
    param.sched_priority = RT_PRIORITY;
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
    
    Logic* engine = static_cast<Logic*>(arg);
    if (!engine->initialize()) return nullptr;
    
    struct timespec deadline;
    clock_gettime(CLOCK_MONOTONIC, &deadline);
    long interval_ns = 1000000000 / LOGIC_HZ; 

    while (keep_running) {
        InputManager::instance().update();
        engine->run_iteration();

        deadline.tv_nsec += interval_ns;
        if (deadline.tv_nsec >= 1000000000) {
            deadline.tv_sec++;
            deadline.tv_nsec -= 1000000000;
        }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL);
    }
    return nullptr;
}

int main(int argc, char *argv[]) {
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sig_pipe)) return 1;
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    mlockall(MCL_CURRENT | MCL_FUTURE);
    
    QApplication app(argc, argv);
    
    ThemeManager::instance();
    InputManager::instance().initialize();

    // PERSISTENCE FIX: Load before UI
    ProfileManager::instance().boot_load();

    QSocketNotifier sn(sig_pipe[0], QSocketNotifier::Read);
    QObject::connect(&sn, &QSocketNotifier::activated, [&]() {
        sn.setEnabled(false);
        char tmp; read(sig_pipe[0], &tmp, 1);
        keep_running = false;
        app.quit();
    });

    Logic engine;
    pthread_t logic_thread;
    pthread_create(&logic_thread, nullptr, logic_thread_func, &engine);

    MainWindow w;
    w.show();

    int result = app.exec();
    keep_running = false;
    pthread_join(logic_thread, nullptr);
    munlockall();
    close(sig_pipe[0]); close(sig_pipe[1]);
    
    return result;
}
