# NetFree

A project built that can bypass the need to pay for a network that would otherwise cost money.  There really is no purpose to this project other than to say I was able to do it.  It acts as an interesting proof of concept and an interesting side project.  If you want to receive notifications about the project, including when the project is released/finished, "Watch" and/or "Star" the project (top-right hand corner of the page below the navbar).

**Status:** _Not Under Active Development_  As explained below, I cannot continue to pursue this project at the moment.

### Inspiration

This project was inspired by something I stumbled upon while searching the interwebs called [Mac-n-Cheese](https://github.com/MarcoPolo/Mac-n-Cheese).  Mac-n-Cheese attempts to bypass networks that would otherwise require money and/or private networks by spoofing one's MAC address with one that has already paid.  Seemingly from Mac-n-Cheese's README, the idea appears to work.  Unfortunately, Mac-n-Cheese can take a while to load.  At the time of writing this, the description for Mac-n-Cheese claims to have taken 8 minutes at one location to find a valid MAC address.

### Disclaimer

As usual this product is available without any guarantees, whether implicit or explicit.  I am also not liable for what may or may not happen using this software.  If you use this software, you MUST acknowledge me as the author around any relevant code.  By using this software you agree to all the above points.  Furthermore, I do not provide this software as a means to bypass paying for web services.  As with any service, companies can rightfully charge you for access to the Internet using their subscription and devices.  While some companies may provide these services free, not all companies can afford to do so or find value in doing so.  Nor is this technology meant to be used as a means to connect to otherwise private networks.  Be respectful of your local laws and others' right to restrict access to their network.  In other words, **do NOT use this software for illegal or immoral activities.**

### How ~does it~ was it supposed to work?

The idea was to listen to the current TCP/IP packets being sent over the network (using ~promiscuous~ radio monitor mode), collecting the MAC address of each packet captured.  As these MAC addresses were collected, the program would rank them based on 2 factors: the number of requests observed from this address and how long ago the last request was received from this address (i.e. the last request's age).  The idea behind counting the number of requests was that authenticated systems should send more TCP requests to the fortified router than unauthenticated systems.  The second factor accounted for routers that granted access for only a limited time.  The weight that each of these factors held was (somewhat) dynamic to account for various types of networks.

Once enough data was collected, the User could cycle through the MAC addresses collected until a feasible one was found (hopefully the first).  Even after the User found a good MAC address, the system would continue to listen for requests over the network, continuously updating its list, just in case the MAC address stopped working.

## What happened?

The idea seems promising, but I don't have access to a system where promiscuous (or monitor) mode actually works.  To test this theory, I installed Wireshark on all my systems and the only traffic that was captured was traffic to/from the system on which I ran Wireshark.

### Next Steps

It would be awesome to continue this project.  I'm actually pretty happy/impressed on how it turned out.  Unfortunately, computers aren't cheap and I'm not sure how to guarantee a computer will actually work in one of the required modes.  So, the true next step (obtaining a system for testing), is unlikely to happen.

However, here are a few additional ideas to improve/enhance the system.  If I obtain a system on which promiscuous/monitor mode work, I will implement them as time presents itself.

- Add actual user input to the main runtime so the program doesn't have to be recompiled when playing around with parameters
- Abstract away any platform-specific code
- Improve the pcap filters to automatically exclude traffic from the current machine

### What did you learn?

This project was pretty neat and I would be interested in actually experimenting with its feasibility.  Even before I realized my system didn't support promiscuous/monitor mode despite it seemingly being able to work in Wireshark, I enjoyed working on this project.  Playing around with networking and lower-level technology was an interesting and welcome change to web development.

Throughout this project, my previous experience working with the Linux kernel and coursework on networking came in handy and was reinforced.  However, much of my coursework with networking was not practical (i.e. did not involve programming), so combining the two was challenging.  Also, my networking course touched on WiFi and the 802.11 protocol, but did not delve much into 802.11 headers.  So, I had to research the protocol a little more in depth.

Finally, working with the pcap library was completely new to me.  Determining the various different ways to configure and start pcap as well as filter its results required research and some cleverness where documentation was sparse.