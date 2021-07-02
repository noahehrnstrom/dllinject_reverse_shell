# dllinject_reverse_shell
This is a reverse shell for windows. The client is programmed in c++ and the server in python. It will inject its own dll into a target program to stay hidden during runtime.

The executable starts by fetching the dll available as a github resource in /bin. It then opens the process RuntimeBroker.exe for full access and injects its dll into the process.
If the server was listening, a new shell should now have been opened to the victim machine.

If you are using 64-bit computer, all the programs should be compiled to x64.

Note that the dll in /bin folder already has a set ip for the target server. Recompile the dll for your server ip if you want to test the shell. Dont forget to modify the url in the client exe as well.
