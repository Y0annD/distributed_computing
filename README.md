# distributed_computing

This application is only an educational purpose on how use multiple computer on a network and distribute long computation.
It allow to break a simple password.

# RUN IT

To run it, it's really simple, type `make` to compile sourcecode, then you can run the server to have an estimated time on password cracking by running
`./decryptServer`.

To run it seriously you should specify a port to listen, if you specify a port below 1024 you must run the server as admin.

    ./decryptServer 5678
    
You can run as much client than your processor have core.
To run a client, you must specify `server_ip_adress` and `server_port`. 
eg: 

    ./decryptClient 192.168.0.11 5678
