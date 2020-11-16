int main (int argc, char **argv) {
    pid_t pid = fork();
    
    if(pid < 0){
        exit(1);
    } else if (pid == 0) {
        //Child
    } else {
        //Parent
    }

    return 0;
}
