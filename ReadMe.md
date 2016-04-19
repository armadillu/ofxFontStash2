#ofxFontStash2

in initial development stages, not working 100% yet.

This includes [fontstash](https://github.com/memononen/fontstash) the [Pugi](https://github.com/zeux/pugixml) xml parser. This can draw both plain strings, or style-formatted paragraph like:

```
string formattedText = "<style font='verdana' size='12' color='#ff0000'>this is red verdana size12</style>";

```
![](https://farm1.staticflickr.com/493/19806237826_788c341f9c_z_d.jpg)

![](https://c2.staticflickr.com/2/1575/26432242845_6f6c18686f.jpg)

# Included Libraries

- [fontstash](https://github.com/memononen/fontstash) by Mikko Mononen: zlib license
- [stb_truetype](https://github.com/nothings/stb) by Sean Barrett: Public domain
- [pugi xml](https://github.com/zeux/pugixml) by Google: MIT License


# GL 3.x compatibility

This is a WIP, but you this can work in GL 3.2 if you define GL_VERSION_3 in your preprocessor macros.  
*GL_3 formated is broken for now, but it draws readable text. It also breaks OF's bitmap font so something is definitively off.