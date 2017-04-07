package com.pomocorp.rnimagetools;

import com.facebook.react.bridge.WritableMap;
import com.facebook.react.bridge.WritableNativeArray;
import com.facebook.react.bridge.WritableNativeMap;

import org.apache.commons.imaging.ImageReadException;
import org.apache.commons.imaging.ImageWriteException;
import org.apache.commons.imaging.Imaging;
import org.apache.commons.imaging.formats.jpeg.JpegImageMetadata;
import org.apache.commons.imaging.formats.png.InterlaceMethod;
import org.apache.commons.imaging.formats.tiff.TiffField;
import org.apache.commons.imaging.formats.tiff.TiffImageMetadata;
import org.apache.commons.imaging.formats.tiff.constants.TiffTagConstants;
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
            WritableNativeMap dir = new WritableNativeMap();

            for (TiffOutputField field : directory.getFields()) {
                TagInfo tagInfo = field.tagInfo;

                TiffField exifValue = jpegImageMetadata.findEXIFValue(tagInfo);
                if (exifValue == null)
                    continue;

                Object value = exifValue.getValueDescription();
                if (value == null)
                    continue;

                dir.putString(tagInfo.name, value.toString());
            }

            map.putMap(directory.description(), dir);
        }

        return map;
    }

    public int orientation() {
        TiffField exifValue = jpegImageMetadata.findEXIFValue(TiffTagConstants.TIFF_TAG_ORIENTATION);
        Object value = exifValue.getValueDescription();
        return value == null ? 0 : Integer.valueOf(value.toString());
    }

    public String timestamp() {
        TiffField exifValue = jpegImageMetadata.findEXIFValue(TiffTagConstants.TIFF_TAG_DATE_TIME);
        Object value = exifValue.getValueDescription();
        return value == null ? null : value.toString();
    }

    public WritableMap location() throws ImageReadException {
        TiffImageMetadata.GPSInfo gps = jpegImageMetadata.getExif().getGPS();

        WritableNativeMap map = new WritableNativeMap();
        map.putDouble("longitude", gps.getLongitudeAsDegreesEast());
        map.putDouble("latitude", gps.getLatitudeAsDegreesNorth());
        return map;
    }
}
