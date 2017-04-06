package com.pomocorp.rnimagetools;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.AssetFileDescriptor;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Environment;
import android.provider.MediaStore;
import android.support.annotation.NonNull;
import android.util.Log;

import com.adobe.creativesdk.aviary.AdobeImageIntent;
import com.adobe.creativesdk.aviary.internal.filters.ToolsFactory;
import com.facebook.common.logging.FLog;
import com.facebook.react.bridge.BaseActivityEventListener;
import com.facebook.react.bridge.Callback;
import com.facebook.react.bridge.Promise;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.ReadableMap;
import com.facebook.react.bridge.WritableNativeMap;
import com.facebook.react.common.ReactConstants;

import java.io.File;
import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.TimeZone;
import java.util.UUID;

import static android.app.Activity.RESULT_CANCELED;
import static android.app.Activity.RESULT_OK;
import static android.provider.MediaStore.Images.*;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.ADJUST;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.BLEMISH;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.BLUR;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.COLOR;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.CROP;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.DRAW;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.EFFECTS;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.ENHANCE;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.FOCUS;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.FRAMES;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.LIGHTING;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.MEME;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.ORIENTATION;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.OVERLAYS;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.PERSPECTIVE;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.REDEYE;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.SHARPNESS;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.SPLASH;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.STICKERS;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.TEXT;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.VIGNETTE;
import static com.adobe.creativesdk.aviary.internal.filters.ToolsFactory.Tools.WHITEN;

public class RNImageToolsModule extends ReactContextBaseJavaModule {
    private static final int REQ_CODE_CSDK_IMAGE_EDITOR = 1122;
    private static final int REQ_CODE_GALLERY_PICKER = 2233;
    private static final String TAG = "rnimagetools";

    private final ReactApplicationContext reactContext;
    private final GalleryListener galleryListener;
    private final EditorListener editorListener;
    private final ContentResolver contentResolver;

    public RNImageToolsModule(ReactApplicationContext reactContext) {
        super(reactContext);
        this.reactContext = reactContext;

        galleryListener = new GalleryListener(REQ_CODE_GALLERY_PICKER);
        reactContext.addActivityEventListener(galleryListener);

        editorListener = new EditorListener(REQ_CODE_CSDK_IMAGE_EDITOR);
        reactContext.addActivityEventListener(editorListener);

        contentResolver = new ContentResolver(reactContext);
    }

    @Override
    public String getName() {
        return "RNImageTools";
    }

    @ReactMethod
    public void checkImageLibraryPermission(Promise promise) {
        //not required for android
        promise.resolve(true);
    }

    @ReactMethod
    public void requestImageLibraryPermission(Promise promise) {
        //not required for android
        promise.resolve(null);
    }

    @ReactMethod
    public void imageMetadata(final String imageUri, final Promise promise) {
        AsyncTask.execute(new Runnable() {
            @Override
            public void run() {
                try {
                    byte[] bytes = contentResolver.readBytes(imageUri);

                    ImageMetadataTools imageMetadataTools = ImageMetadataTools.createImageMetadata(bytes);

                    promise.resolve(imageMetadataTools.asMap());
                } catch (Exception e) {
                    Log.e(TAG, "failed to get metadata from  " + imageUri, e);
                    promise.reject("error", "metadata extract failed", e);
                }
            }
        });
    }

    @ReactMethod
    public void authorize(String clientId, String clientSecret, String clientUri) {
        //ignored for android
    }

    @ReactMethod
    public void selectImage(ReadableMap options, Promise promise) {
        Intent galleryPickerIntent = new Intent();
        galleryPickerIntent.setType("image/*");//limit to images for now
        galleryPickerIntent.setAction(Intent.ACTION_GET_CONTENT);

        String title = options.hasKey("title") ? options.getString("title") : "choose image";//where does this appear?

        galleryListener.add(promise);

        getReactApplicationContext().startActivityForResult(Intent.createChooser(galleryPickerIntent, title), REQ_CODE_GALLERY_PICKER, null);
    }

    @ReactMethod
    public void openEditor(ReadableMap options, Promise promise) {
        try {
            Uri imageUri;
            if (options.hasKey("imageUri"))
                imageUri = Uri.parse(options.getString("imageUri"));
            else {
                promise.reject("error", "imageUri not present");
                return;
            }

            Bitmap.CompressFormat format = Bitmap.CompressFormat.JPEG;
            if (options.hasKey("outputFormat")) {
                format = Bitmap.CompressFormat.valueOf(options.getString("outputFormat"));
            }

            int quality = 80;
            if (options.hasKey("quality")) {
                quality = options.getInt("quality");
            }

            String saveTo = "photos";
            if (options.hasKey("saveTo")) {
                saveTo = options.getString("saveTo");
            }

            editorListener.add(promise);

            AdobeImageIntent.Builder builder = new AdobeImageIntent.Builder(reactContext)
                    .setData(imageUri)
                    .withToolList(new ToolsFactory.Tools[]{
                            CROP,
                            ORIENTATION,
                            ENHANCE,
                            TEXT,
                            DRAW,
                            MEME,
                            FOCUS,
                            VIGNETTE,
                            SHARPNESS,
                            BLUR,
                            COLOR,
                            EFFECTS,
                            ADJUST,
                            PERSPECTIVE,
                            REDEYE,
                            WHITEN,
                            BLEMISH,
                            SPLASH,
                            LIGHTING,
                            OVERLAYS,
                            FRAMES,
                            STICKERS,
                    })
                    .withOutputFormat(format)
                    .withOutputQuality(quality)
                    .withNoExitConfirmation(true);

            if (saveTo.equals("photos")) {
                File dir = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM);
                builder.withOutput(new File(dir, "rnimagetools." + UUID.randomUUID().toString() + "." + format.name()));
            } else {
                builder.withOutput(File.createTempFile("rnimagetools.", "." + format.name(), reactContext.getCacheDir()));
            }

            getReactApplicationContext().startActivityForResult(builder.build(), REQ_CODE_CSDK_IMAGE_EDITOR, null);
        } catch (Exception e) {
            promise.reject("error", "failed to start editor", e);
        }
    }

    @NonNull
    private WritableNativeMap imageData(Uri uri) {
        WritableNativeMap response = new WritableNativeMap();

        String[] projection = {
                Media._ID,
                Media.MIME_TYPE,
                Media.BUCKET_DISPLAY_NAME,
                Media.SIZE,
                Media.ORIENTATION,
                Media.DISPLAY_NAME,
                Media.DATE_TAKEN,
                Media.TITLE,
                Media.WIDTH,
                Media.HEIGHT,
                Media.LONGITUDE,
                Media.LATITUDE
        };

        android.content.ContentResolver resolver = reactContext.getContentResolver();
        Cursor cursor = resolver.query(uri, projection, null, null, null);

        try {
            if (cursor != null && cursor.getCount() == 1) {
                cursor.moveToFirst();
                String mimeType = cursor.getString(cursor.getColumnIndex(Media.MIME_TYPE));
                response.putString("mimeType", mimeType);

                response.putInt("size", cursor.getInt(cursor.getColumnIndex(Media.SIZE)));
                response.putInt("orientation", cursor.getInt(cursor.getColumnIndex(Media.ORIENTATION)));
                response.putString("filename", cursor.getString(cursor.getColumnIndex(Media.DISPLAY_NAME)));

                long timestamp = cursor.getLong(cursor.getColumnIndex(Media.DATE_TAKEN));
                response.putString("timestamp", formatDate(timestamp));

                {
                    int width = cursor.getInt(cursor.getColumnIndex(Media.WIDTH));
                    int height = cursor.getInt(cursor.getColumnIndex(Media.HEIGHT));

                    if (width <= 0 || height <= 0) {
                        try {
                            AssetFileDescriptor photoDescriptor = resolver.openAssetFileDescriptor(uri, "r");
                            BitmapFactory.Options options = new BitmapFactory.Options();
                            options.inJustDecodeBounds = true;
                            BitmapFactory.decodeFileDescriptor(photoDescriptor.getFileDescriptor(), null, options);
                            photoDescriptor.close();

                            width = options.outWidth;
                            height = options.outHeight;
                        } catch (IOException e) {
                            FLog.e(TAG, "Could not get dimensions for " + uri.toString(), e);
                        }
                    }

                    WritableNativeMap map = new WritableNativeMap();
                    map.putDouble("width", width);
                    map.putDouble("height", height);
                    response.putMap("dimensions", map);
                }

                double longitude = cursor.getDouble(cursor.getColumnIndex(Media.LONGITUDE));
                double latitude = cursor.getDouble(cursor.getColumnIndex(Media.LATITUDE));
                if (longitude > 0 || latitude > 0) {
                    WritableNativeMap map = new WritableNativeMap();
                    map.putDouble("longitude", longitude);
                    map.putDouble("latitude", latitude);
                    response.putMap("location", map);
                }
            }
        } finally {
            if(cursor != null)
                cursor.close();
        }
        return response;
    }

    private abstract class BaseListener extends BaseActivityEventListener {
        Promise callback;
        private final int requestCode;

        public BaseListener(int requestCode) {
            this.requestCode = requestCode;
        }

        @Override
        public void onActivityResult(Activity activity, int requestCode, int resultCode, Intent data) {
            if (requestCode != this.requestCode)
                return;

            if (resultCode == RESULT_CANCELED) {
                //cool - they pressed the back button
                resolve(null);
            } else if (resultCode == RESULT_OK) {
                if (data == null) {
                    reject("no data");
                    return;
                }

                Uri uri = uriFrom(data);

                if (uri != null) {
                    resolve(uri);
                } else {
                    reject("no output uri");
                }
            } else {
                reject("unrecognised return code: " + resultCode);
            }
        }

        void resolve(Uri uri) {
            String realPathFromURI = uri.toString();
            if(callback != null) {
                callback.resolve(realPathFromURI);
            }
        }

        void reject(String reason) {
            if(callback != null) {
                callback.reject("error", reason);
            }
        }

        protected abstract Uri uriFrom(Intent data);

        public void add(Promise promise) {
            this.callback = promise;
        }
    }

    private class GalleryListener extends BaseListener {
        public GalleryListener(int code) {
            super(code);
        }

        @Override
        protected Uri uriFrom(Intent data) {
            return data.getData();
        }

        @Override
        void resolve(Uri uri) {
            WritableNativeMap response = imageData(uri);
            response.putString("uri", uri.toString());

            if(callback != null) {
                callback.resolve(response);
            }
        }

    }

    private class EditorListener extends BaseListener {

        public EditorListener(int code) {
            super(code);
        }

        @Override
        protected Uri uriFrom(Intent data) {
            return data.getParcelableExtra(AdobeImageIntent.EXTRA_OUTPUT_URI);
        }
    }

    private static String formatDate(long timestamp) {
        DateFormat df = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm'Z'", Locale.getDefault());
        df.setTimeZone(TimeZone.getTimeZone("UTC"));
        return df.format(new Date(timestamp));
    }
}