# NetFree

**This project is still under development.** A project built that can bypass the need to pay for a network that would otherwise cost money.  There really is no purpose to this project other than to say I was able to do it.  It acts as an interesting proof of concept and an interesting side project.  If you want to receive notifications about the project, including when the project is released/finished, "Watch" and/or "Star" the project (top-right hand corner of the page below the navbar).

**Status: ** _Under Development_  If you are interested in this project, please "Watch" and/or "Star" the project.  This will encourage me to continue developing this project over others I have in the pipeline.

### Inspiration

This project was inspired by something I stumbled upon while searching the interwebs called [Mac-n-Cheese](https://github.com/MarcoPolo/Mac-n-Cheese).  Mac-n-Cheese attempts to bypass networks that would otherwise require money and/or private networks by spoofing one's MAC address with one that has already paid.  Unfortunately, Mac-n-Cheese can take a while to load.  At the time of writing this, the description for Mac-n-Cheese claims to have taken 8 minutes at one location to find a valid MAC address.

### Disclaimer

As usual this product is available without any guarantees, whether implicit or explicit.  I am also not liable for what may or may not happen using this software.  If you use this software, you MUST acknowledge me as the author around any relevant code.  By using this software you agree to all the above points.  Furthermore, I do not provide this software as a means to bypass paying for web services.  As with any service, companies can rightfully charge you for access to the Internet using their subscription and devices.  While some companies may provide these services free, not all companies can afford to do so or find value in doing so.  Nor is this technology meant to be used as a means to connect to otherwise private networks.  Be respectful of your local laws and others' right to restrict access to their network.  In other words, **do NOT use this software for illegal or immoral activities.**

### How does it work?

I don't know, yet.  Let me write the program first!  The idea, though, is to listen to current WiFi signals being sent to the fortified router in promiscuous mode and grab valid MAC addresses from there.  ~Falling back to Mac-and-Cheese's method, if necessary.~  Mac-n-Cheese using nmap to scan the network, which probably isn't the optimal solution.  Besides, this approach has shown promise thus far.

### How to Use

I'll get to this later.

### Next Steps

Here are a few additional ideas to improve/enhance the system.  If I obtain enough requests for them, I will implement them as time presents itself.

- Abstract away the platform using a bridge/facade inspired design

### Questions

**Why did you create your own testing framework instead of using cmocka?**  For the most part, I don't like cmocka's notation (or documentation).  Furthermore, its code is pretty atrocious: everything is in the same file and kinda convoluted.  In my opinion, not even what it's supposed to do well (mocking functions) does it do well.  However, I have used quite a few testing frameworks before and I figured I could create a better assertion library and testing framework.  The result?  Something similar to what you might expect to see in JavaScript (using Jasmine/Karma or Mocha) or other higher level language (maybe even [Java](http://www.mscharhag.com/java/oleaster-jasmine-junit-tests)).  Originally, I had hoped to use cmocka's mocking library; however, after looking more into the library I determined I didn't like their approach anymore than the one I originally thought of using to mock functions (i.e. using wrapper functions the same way I mock `*alloc()` and `free()`).  Since this appears to be the best/only approach, I plan on adding this feature eventually to my testing framework.

**Why didn't you use [_insert favorite OO programming language_]?**  Good question.  Hopefully my answer will be equally well-received.  I enjoy using C as it is a challenge, especially to write good, clean code.  Have you ever thought, "Hmm... I bet I can structure my C code similar to how I would using OOP or make something like that (_insert higher-level programming language construct like try...catch here_)?"  Or have you ever looked at the code available for C libraries and were frightened by the mess that could have been structured much nicer?  Well, I have and I like challenging myself to do so.  However, the code should be written in such a way that would make it extremely easy to convert to a C++ (or other OO language) equivalent implementation.  Who knows, I just might try that, eventually.

**I can't run the (tests|code).  Help!**  That's not really a question, but if the problem is with the tests chances are you need to install libffcall.  For me, that meant running `sudo apt-get install libffcall1-dev`.  If that doesn't work, well, I'm good, but I can't debug the future without knowing something about the problem.  Just open an issue on this page with details of what lead to the problem and any debugging information available.
