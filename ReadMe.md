# ofxFontStash2

[![Build Status](https://travis-ci.org/armadillu/ofxFontStash2.svg?branch=master)](https://travis-ci.org/armadillu/ofxFontStash2)

![https://farm5.staticflickr.com/4186/34677341675_d882b5797f_o_d.png](https://farm5.staticflickr.com/4186/34677341675_def1bee422_z_d.jpg)



ofxFontStash2 allows you to draw text. It can draw individual lines of text, or longer texts constrained to a column (to create paragraph, automatically flowing the text for you). 

It offers text alignment (left, center, right).

It also allows you to mix and match different text styles in the same paragraph. You can do so by creating styles that can be applied at an individual word level. You can also inline styles;

Drawing this "formatted" text paragraphs is accomplished with an HTML style syntax (xml), for example:


```
string formattedText = 
	
	//style based on predefined style
	"<style id='style1'>this is my text in style1</style>";
	
	//inline defined style
	"<style font='verdana' size='12' color='#ff0000'>this is red verdana size12</style>";

```

It also supports emoji characters (b&w only for now) that you can mix with your main font. You supply your own emoji font together with your main font, and the right pictogram is chosen through a fallback paradigm. If your main font doesn't supply a glyph for an certain unicode sequence, ofxFontStash2 will try fetch it from the fallback font (your emoji font) which most likely have it.

It handles tabs as expected, so you can use it to draw monospaced tabulated code.

It also supports multiple device resolutions, so for example moving your project to a "retina" screen should take zero effort.

![https://farm5.staticflickr.com/4403/35538430564_af59b4c811_o_d.png](https://farm5.staticflickr.com/4403/35538430564_77a8d9e1a7_z_d.jpg)

# Formatting Options

### 1. With a predefined Style

```
<style id='myStyleID'>Text goes here</style>
 or
<myStyleID>Text goes here</myStyleID>
```

### 2. With a predefined Style with overrides

```
<style id='myStyleID' override1='value1' override2='value2'>Text goes here</style>
 or 
<myStyleID override1='value1' override2='value2'>Text goes here</myStyleID>
```

Valid overrides are ```color```, ```size ```, ```blur ``` and ```font ```.

Example: ```<myStyleID override='value' color='#ff' size='22' blur='3' font='MyFontID'>Text goes here</myStyleID>```


### 3. Colors
must be defined in hex: several options are accepted; RGBA ```#AABBCCDD```, RGB ```#AABBCCDD``` and GRAY ```#AA```.

### 4. \<BR/>
You can include line breaks by adding ```<br/>```.  

Example: ```<myStyle>here's a line<br/>break<myStyle>```

The BR tag also accepts a lineHeight multiplier, in those cases where you want a custom height line break;  
Example: ```<MyStyle>This Line break<br/><br heightMult='1.5'/>is 1.5 times taller than normal.</MyStyle>```  


# Included Libraries

- [nanovg](https://github.com/memononen/nanovg) by Mikko Mononen: zlib license - with tiny modifications to avoid duplicated symbols
- [fontstash](https://github.com/memononen/nanovg) by Mikko Mononen: zlib license - with tiny modifications to avoid duplicated symbols
- [stb_truetype](https://github.com/nothings/stb) by Sean Barrett: Public domain
- [stb_image](https://github.com/nothings/stb) by Sean Barrett: Public domain
- [pugi xml](https://github.com/zeux/pugixml) by Google: MIT License

# GL2, GL3 and GLES compatibility

To use in in a GL* context, define the appropriate preprocessor macro in your project;

```
OpenGL 2.x >> NANOVG_GL2_IMPLEMENTATION
OpenGL 3.x >> NANOVG_GL3_IMPLEMENTATION
OpenGL ES  >> NANOVG_GLES2_IMPLEMENTATION
```


# License

ofxFontStash2 has been created by Oriol Ferrer Mesià and Hans Raber and is released under [MIT license](http://www.opensource.org/licenses/mit-license.php).

	Copyright (c) 2017 Oriol Ferrer Mesià & Hans Raber
	
	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
