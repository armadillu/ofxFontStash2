#ofxFontStash2

in initial development stages, not working yet.

This includes [fontstash](https://github.com/memononen/fontstash) the [Gumbo](https://github.com/google/gumbo-parser) html parser. This can draw both plain strings, or style-formatted paragraph like:

```
string formattedText = "<style font=verdana size=12 color=#ff0000>this is red verdana size12</style>";

```
![](https://farm1.staticflickr.com/493/19806237826_788c341f9c_z_d.jpg)


Note! apparently the Gumbo Parser does NOT compile under Visual Studio!

# Included Libraries

- [fontstash](https://github.com/memononen/fontstash) by Mikko Mononen: zlib license
- [stb_truetype](https://github.com/nothings/stb) by Sean Barrett: Public domain
- [gumbo parser](https://github.com/google/gumbo-parser) by Google: Apache license
- [utf-8 decoder](http://bjoern.hoehrmann.de/utf-8/decoder/dfa/) by Bjoern Hoehrmann: MIT license
