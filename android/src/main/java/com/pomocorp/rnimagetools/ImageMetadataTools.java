package com.pomocorp.rnimagetools;

import com.facebook.react.bridge.WritableMap;
import com.facebook.react.bridge.WritableNativeArray;
import com.facebook.react.bridge.WritableNativeMap;

import org.apache.commons.imaging.ImageReadException;
import org.apache.commons.imaging.ImageWriteException;
import org.apache.commons.imaging.Imaging;
import org.apache.commons.imaging.formats.jpeg.JpegImageMetadata;
import org.apache.commons.imaging.formats.tiff.TiffField;
import org.apache.commons.imaging.formats.tiff.taginfos.TagInfo;
import org.apache.commons.imaging.formats.tiff.write.TiffOutputDirectory;
import org.apache.commons.imaging.formats.tiff.write.TiffOutputField;
import org.apache.commons.imaging.formats.tiff.write.TiffOutputSet;

import java.io.IOException;

public class ImageMetadataTools {
    private JpegImageMetadata jpegImageMetadata;

    public ImageMetadataTools(JpegImageMetadata jpegImageMetadata) {
        this.jpegImageMetadata = jpegImageMetadata;
    }

    public static ImageMetadataTools createImageMetadata(byte[] bytes) throws IOException, ImageReadException {
        JpegImageMetadata jpegImageMetadata = (JpegImageMetadata) Imaging.getMetadata(bytes);
        return new ImageMetadataTools(jpegImageMetadata);
    }

    public WritableMap asMap() throws ImageWriteException {
        TiffOutputSet exifData = jpegImageMetadata.getExif().getOutputSet();

        WritableNativeMap map = new WritableNativeMap();

        for (TiffOutputDirectory directory : exifData.getDirectories()) {
            WritableNativeArray dir = new WritableNativeArray();

            for (TiffOutputField field : directory.getFields()) {
                TagInfo tagInfo = field.tagInfo;

                TiffField exifValue = jpegImageMetadata.findEXIFValue(tagInfo);
                if (exifValue == null)
                    continue;

                Object value = exifValue.getValueDescription();
                if (value == null)
                    continue;

                WritableNativeMap f = new WritableNativeMap();
                f.putString("type", field.fieldType.getName());
                f.putString("name", tagInfo.name);
                f.putString("value", value.toString());
                dir.pushMap(f);
            }

            map.putArray(directory.description(), dir);
        }

        return map;
    }
}
