# react-native-image-tools

Install instructions [here](./docs/install.md).

An example app is [here](https://github.com/npomfret/rn-image-tools-example).

<img src="docs/demo.gif" width="350">

### API


_RNImageTools.openEditor(options)_ returns a promise which when resolved contains a uri to the output image, or null if editing was cancelled.

| name | description | default value | type |
| :---: | :---: | :---: | :--- |
| imageUri | the input image | n/a | `string`, supported formats include _path_ (e.g. `/foo/bar/image.jpeg`), _file url_ (e.g. `file:///foo/bar/image.jpeg`), _url_ (e.g. `http://foo.com/bar/image.jpeg`, _asset-uri_ (iOS ony, e.g. `assets-library://asset/asset.JPG?id=foo&EXT=JPG`), _content-uri_ (android only`content://media/external/images/foo/bar/JPEG`) |
| outputFormat | the image type, must correspond to the input image type (generating PNG from JPEG or vider-vesa is not supported) | JPEG | `string`, either _'JPEG'_ or _'PNG'_ |
| quality | representing the JPEG compression % (Ignored for PNG) | 80 | `integer`, a value between _0_ and _100_ |
| preserveMetadata | copies image metadata (exif tags) from the input image to the output image | true | `boolean` |
| saveTo | determines where the output image will be saved (`temp` writes to the application temp folder, `photos` writes to the default image store on the device | photos | `string`, _'photos'_ or _'temp'_ |

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

      console.log("edited uri", uri);
    } catch (e) {
      console.warn("error", e);
    }
```

### todo

 * preserve image metadata on android (in progress)
 * merge metadata, rather than overwrite
 * customise the tool array https://github.com/CreativeSDK/phonegap-plugin-csdk-image-editor/blob/master/src/ios/CDVImageEditor.m#L49
 * support more oof the underlying Adobe API...  