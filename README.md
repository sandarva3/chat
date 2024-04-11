# Real-Time Chat System on Local Network

# OverView
This is a C program enables real-time communication between users on a local network.

## Getting Started
By running the `client.exe` file, users can connect to a shared hotspot and chat seamlessly.
Server file code is meant to be run/compiled on Windows OS.
There's two files for client side code:
- **Wclient**: It's meant to run/compiled on Windows OS.
- **Aclient**: I modified the Windows code little bit and made it compatible with unix-like OS, to run it on Android.

Modify the client codes by entering the correct IP address of your server and compile them.
I kept number of clients that can be connected as 4, you can edit it.
Run `server.exe` and `client.exe` file and open hotspot, and let other devices run `client.exe` file and connect them to your hotspot.
Client would be required to enter their name.
Server would see the IP address of each client connected but client won't see any IP addresses, just a name of another client on group chat.

Now you're all set to have group chat on your own network.

## Contributing
Feel free to enhance this project by adding more useful awesome features.
