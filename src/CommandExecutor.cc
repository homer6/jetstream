#include "CommandExecutor.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <cstring>
#include <map>
#include <vector>
#include <thread>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <poll.h>
#include <stdexcept>

extern char **environ; // For accessing the environment

namespace jetstream {

    CommandExecutor::CommandExecutor() {
        // Default constructor
    }

    CommandExecutor::CommandExecutor( const std::string& command ){
        this->addArguments( command );
    }

    void CommandExecutor::addArgument( const std::string& arg ){
        args.addArg(arg);
    }

    void CommandExecutor::addArguments( const std::string& args_string ){
        args.addArgs( args_string );
    }

    void CommandExecutor::addArguments( const std::vector<std::string>& args_vector ){
        this->args.clear();
        for( const std::string& arg : args_vector ){
            this->addArgument( arg );
        }
    }

    void CommandExecutor::addEnvironmentVariable( const std::string& key, const std::string& value ){
        std::string env_var = key + "=" + value;
        env.addArg( env_var );
    }

    void CommandExecutor::addEnvironmentVariables( const std::map<std::string, std::string>& env_vars ){
        env.clear();
        for( const auto& [key, value] : env_vars ){
            addEnvironmentVariable(key, value);
        }
    }

    void CommandExecutor::setStdoutCallback(const std::function<void(const std::string&)>& callback) {
        stdout_callback = callback;
    }

    void CommandExecutor::setStderrCallback(const std::function<void(const std::string&)>& callback) {
        stderr_callback = callback;
    }


    int CommandExecutor::execute( bool wait_for_completion, const std::function<void()>& poll_service_callback ){

        int stdout_pipe[2];
        int stderr_pipe[2];

        if( pipe(stdout_pipe) == -1 ){
            throw std::runtime_error("Failed to create stdout pipe: " + std::string(strerror(errno)));
        }
        if( pipe(stderr_pipe) == -1 ){
            throw std::runtime_error("Failed to create stderr pipe: " + std::string(strerror(errno)));
        }

        pid_t pid = fork();

        if( pid == -1 ){

            throw std::runtime_error(std::string(strerror(errno)));

        }else if( pid == 0 ){

            // Child process

            // Redirect stdout
            dup2(stdout_pipe[1], STDOUT_FILENO);
            close(stdout_pipe[0]);
            close(stdout_pipe[1]);

            // Redirect stderr
            dup2(stderr_pipe[1], STDERR_FILENO);
            close(stderr_pipe[0]);
            close(stderr_pipe[1]);

            // Close parent's pipe ends
            // Close other file descriptors if necessary

            //char **envp = env.size() > 0 ? env.data() : environ;
            if( execve( args.data()[0], args.data(), env.data() ) == -1 ){
                perror("execve");                
                _exit(errno); // Use _exit to avoid flushing stdio buffers
            }

        } else {

            // Parent process

            // Close unused pipe ends
            close(stdout_pipe[1]);
            close(stderr_pipe[1]);

            std::thread stdout_thread;
            std::thread stderr_thread;

            // Read from stdout pipe
            if( stdout_callback ){
                stdout_thread = std::thread([this, stdout_pipe]() {
                    char buffer[4096];
                    ssize_t count;
                    while ((count = read(stdout_pipe[0], buffer, sizeof(buffer))) > 0) {
                        std::string output(buffer, count);
                        stdout_callback(output);
                    }
                    close(stdout_pipe[0]);
                });                
            } else {
                // No callback, close the read end
                close(stdout_pipe[0]);
            }

            // Read from stderr pipe
            if( stderr_callback ){
                stderr_thread = std::thread([this, stderr_pipe](){
                    char buffer[4096];
                    ssize_t count;
                    while ((count = read(stderr_pipe[0], buffer, sizeof(buffer))) > 0) {
                        std::string output(buffer, count);
                        stderr_callback(output);
                    }
                    close(stderr_pipe[0]);
                });
            } else {
                // No callback, close the read end
                close(stderr_pipe[0]);
            }

            int status = 0;

            if( wait_for_completion ){

                // Timeout loop to wait for child process and service poll
                while (true) {

                    pid_t result = waitpid(pid, &status, WNOHANG); // Non-blocking wait

                    if (result == 0) {
                        // Child process still running, sleep for a second (or less)
                        std::this_thread::sleep_for(std::chrono::seconds(1));

                        // Here you can call your poll-servicing lambda function
                        if( poll_service_callback ){
                            poll_service_callback(); // Call the poll service logic
                        }

                    } else if (result == -1) {

                        // Error in waitpid
                        throw std::runtime_error( std::string(strerror(errno)) );

                    } else {

                        // Child process has finished
                        break;

                    }

                }

                // Wait for threads to finish
                if( stdout_thread.joinable() ){
                    stdout_thread.join();
                }
                if( stderr_thread.joinable() ){
                    stderr_thread.join();
                }

                if( WIFEXITED(status) ){
                    return WEXITSTATUS(status);
                }else if( WIFSIGNALED(status) ){
                    int signal = WTERMSIG(status);
                    throw std::runtime_error("Child process terminated by signal: " + std::to_string(signal));
                }else{
                    throw std::runtime_error("Child process did not exit normally");
                }

            } else {

                // Do not wait for completion
                // Threads will continue to run; ensure that CommandExecutor remains in scope
                return 0;

            }

        }

        // Should not reach here
        return 0;
    }

}
