**Unfortunately Adobe has discontinued development and support of the Image Editor UI this project is discontinued.**

# ~~react-native-image-tools~~

Install instructions [here](./docs/install.md).

An example app is [here](https://github.com/npomfret/rn-image-tools-example).

<img src="docs/demo.gif" width="350">

## API

### _RNImageTools.openEditor(options)_

Opens the Adobe Image Editor.  Returns a promise which when resolved contains a uri to the output image, or null if editing was cancelled.

_options_ parameter:

| name | description | default value | type |
| :---: | :---: | :---: | :--- |
| imageUri | the input image | n/a | `string` (see below)|
| outputFormat | the image type, must correspond to the input image type (generating PNG from JPEG or vider-vesa is not supported) | JPEG | `string` (either _'JPEG'_ or _'PNG')_ |
| quality | representing the JPEG compression % (Ignored for PNG) | 80 | `integer` (a value between _0_ and _100_) |
| preserveMetadata (iOS only) | copies image metadata (exif tags) from the input image to the output image | true | `boolean` |
| saveTo | determines where the output image will be saved | photos | `string` (_'photos'_ or _'file'_) |

Supported `imageUri` formats include:
 * _path_ e.g. `/foo/bar/image.jpeg`
 * _file url_ e.g. `file:///foo/bar/image.jpeg`
 * _url_ e.g. `http://foo.com/bar/image.jpeg`
 * _asset-uri_ (iOS only) e.g. `assets-library://asset/asset.JPG?id=foo&EXT=JPG`
 * _content-uri_ (android only) e.g. `content://media/external/images/foo/bar/JPEG`
 
```javascript
    import RNImageTools from "react-native-image-tools";

    ...    

    try {
      const uri = await RNImageTools.openEditor({
        imageUri,
        outputFormat,
        quality,
        preserveMetadata,
        saveTo
      });
    } catch (e) {
      console.warn("error", e);
    }
```

### _RNImageTools.selectImage(options)_

Opens the native image picker dialog. Returns a promise which when resolved returns an object containing the uri to the chosen image as well as some other useful information, or null if cancelled.

_options_ parameter:

| name | description | default value | type |
| :---: | :---: | :---: | :--- |
| title | optional title (android only) | n/a | `string` |

```javascript
    import RNImageTools from "react-native-image-tools";

    ...    

    try {
      const uri = await RNImageTools.selectImage({title});
    } catch (e) {
      console.warn("error", e);
    }
```

### _RNImageTools.imageMetadata(imageUri)_

Returns a promise which when resolved returns object containing the image metadata.

```javascript
    import RNImageTools from "react-native-image-tools";

    ...    

    try {
      const metadata = await RNImageTools.imageMetadata(imageUri);
    } catch (e) {
      console.warn("error", e);
    }
```

### todo

 * add removeMetadata method
 * support file urls in imageData method
 * customise the tool array https://github.com/CreativeSDK/phonegap-plugin-csdk-image-editor/blob/master/src/ios/CDVImageEditor.m#L49
 * support more oof the underlying Adobe API...  