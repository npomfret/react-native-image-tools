# react-native-image-tools

Install instructions [here](./docs/install.md).

An example app is [here](https://github.com/npomfret/rn-image-tools-example).

<img src="docs/demo.gif" width="350">

### API


_RNImageTools.openEditor(options)_ returns a promise which when resolved contains a uri to the output image, or null if editing was cancelled.

| name | default value | |
| ------------- |:-------------:| -----:|
| imageUri | n/a | supported formats include _path_ (e.g. `/foo/bar/image.jpeg`), _file url_ (e.g. `file:///foo/bar/image.jpeg`), _url_ (e.g. `http://foo.com/bar/image.jpeg`, _asset-uri_ (iOS ony, e.g. `assets-library://asset/asset.JPG?id=foo&EXT=JPG`), _content-uri_ (android only`content://media/external/images/foo/bar/JPEG`) |
| outputFormat | JPEG | allowed values are `JPEG` or `PNG` - must correspond to the input image type (generating PNG from JPEG or vider-vesa is not supported) |
| quality | 80 (Ignored for PNG)| integer value between 0 and 100 representing the JPEG compression % |
| preserveMetadata | n/a | copies image metadata (exif tags) from the input image to the output image |
| saveTo | photos | allowed values are `photos` or `temp` - determines where the output image will be saved (`temp` writes to the application temp folder, `photos` writes to the default image store on the device  |

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

 * preserve metadata
   * android support
 * support more oof the underlying Adobe API...  