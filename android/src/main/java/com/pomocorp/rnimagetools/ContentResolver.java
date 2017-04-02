package com.pomocorp.rnimagetools;

import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.provider.MediaStore;

import org.apache.commons.io.FileUtils;
import org.apache.commons.io.IOUtils;

import java.io.File;
import java.io.IOException;
import java.net.URL;

public class ContentResolver {
    private Context reactContext;

    public ContentResolver(Context reactContext) {
        this.reactContext = reactContext;
    }

    public String resolveUri(Uri uri) {
        /*
        content://com.android.providers.media.documents/document/image%3A519
        content://media/external/images/media/430
        ... or maybe (like from dropbox)
        file:///storage/emulated/0/Android/data/com.dropbox.android/files/u3578267/scratch/testing/2015-04-26%2017.00.22.jpg
         */

        String scheme = uri.getScheme();
        if (scheme == null) {
            return uri.getPath();
        } else if ("file".equals(scheme)) {
            return uri.getPath();
        } else if ("content".equals(scheme)) {
            Cursor cursor = reactContext.getContentResolver().query(uri, new String[]{MediaStore.Images.Media.DATA}, null, null, null);

            if (cursor == null) { // not from the local database
                return uri.toString();
            } else {
                cursor.moveToFirst();
                try {
                    int idx = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
                    String filePath = cursor.getString(idx);
                    return new File(filePath).toURI().toString();
                } finally {
                    cursor.close();
                }
            }
        } else {
            return uri.toString();
        }
    }

    public byte[] readBytes(File file) throws IOException {
        return readBytes(file.getAbsolutePath());
    }

    public byte[] readBytes(String uri) throws IOException {
        String realFilePath = resolveUri(Uri.parse(uri));

        if (realFilePath.startsWith("/") || realFilePath.startsWith("file:/")) {
            File file = new File(Uri.parse(realFilePath).getPath());
            return FileUtils.readFileToByteArray(file);
        } else {
            return IOUtils.toByteArray(new URL(realFilePath));
        }
    }
}
