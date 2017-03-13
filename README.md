# NetFree

A project built that can bypass the need to pay for a network that would otherwise cost money.  There really is no purpose to this project.  It acts as an interesting proof of concept and an interesting side project.

### Inspiration

This project was inspired by something I stumbled upon while searching the interwebs called [Mac-n-Cheese](https://github.com/MarcoPolo/Mac-n-Cheese).  Mac-n-Cheese attempts to bypass networks that would otherwise require money and/or private networks by spoofing one's MAC address with one that is has already paid.  Unfortunately, Mac-n-Cheese can take a while to load.  At the time of writing this, the description for Mac-n-Cheese claims to have taken 8 minutes at one location to find a valid MAC address.

### Disclaimer

As usual this product is available without any guarantees, whether implicit or explicit.  It might not work or it could tear a hole in the fabric of reality and leave twinkies all over time or even cause aliens from the space between space to attack and enslave all humans... use at your own risk.  Furthermore, I do not provide this software as a means to bypass paying for web services.  As with any service, companies can rightfully charge you for access to the Internet using their subscription and devices.  While some companies may provide these services free, not all companies can afford to do so or find value in doing so.  Nor is this technology meant to be used as a means to connect to otherwise private networks.  Be respectful of your local laws and others' right to restrict access to their network.

### How does it work?

I don't know, yet.  Let me write the program first!  The idea, though, is to listen to current WiFi signals being sent to the fortified router in promiscuous mode and grab valid MAC addresses from there.  Falling back to Mac-and-Cheese's method, if necessary.

### Requirements

1. Examine the code.  Never trust code pulled from the Internet that's free.  A list of a couple of things you may want to modify for your system will appear in the *How to Use* section.
2. Let me know how it worked for you and if you noticed any bugs.  
3. If you use the code, give me credit.

### How to Use

I'll get to this later.

### Questions

*Why did you create your own testing framework and use cmocka?*  For the most part, I don't like cmocka's notation.  Furthermore, its code is pretty atrocious: everything is in the same file and kinda convoluted.  That's actually the reason I ended using cmocka.  Instead of trying to reverse engineer how exactly they mocked functions, I determined the time necessary wasn't worth the reward.  However, I have used quite a few testing frameworks before and I figured I could create a better assertion library and testing framework.  The result?  Something similar to what you might expect to see in JavaScript (using Jasmine/Karma or Mocha) or other higher level language (maybe even (Java)[http://www.mscharhag.com/java/oleaster-jasmine-junit-tests]).

*Why didn't you use [_insert favorite OO programming language_]?*  Good question.  Hopefully my answer will be equally well-received.  I enjoy using C as it is a challenge, especially to write good, clean code.  Have you ever thought, "Hmm... I bet I can structure my C code similar to how I would using OOP?"  Or have you ever looked at the code available for C libraries and were frightened by the mess that could have been structured much nicer?  Well, I have and I like challenging myself to do so.  However, the code should be written in such a way that would make it extremely easy to convert to a C++ (or other OO language) equivalent implementation.  Who knows, I just might try that, eventually.

* I can't run the (tests|code).  Help!*  That's not really a question, but if the problem is with the tests chances are you need to install libffcall.  For me, that meant running `sudo apt-get install libffcall1-dev`.  If that doesn't work, well, I'm good, but I can't debug the future without knowing something about the problem.  Just open an issue on this page with details of what lead to the problem and any debugging information available.