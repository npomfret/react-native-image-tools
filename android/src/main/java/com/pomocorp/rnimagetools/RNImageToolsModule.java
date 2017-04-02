package com.pomocorp.rnimagetools;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.media.MediaScannerConnection;
import android.net.Uri;
import android.os.AsyncTask;
import android.util.Log;

import com.adobe.creativesdk.aviary.AdobeImageIntent;
import com.adobe.creativesdk.aviary.internal.filters.ToolsFactory;
import com.facebook.react.bridge.BaseActivityEventListener;
import com.facebook.react.bridge.Promise;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.ReadableMap;
import com.facebook.react.bridge.WritableMap;

import org.apache.commons.imaging.ImageReadException;
import org.apache.commons.imaging.ImageWriteException;
import org.apache.commons.imaging.Imaging;
import org.apache.commons.imaging.common.ImageMetadata;
import org.apache.commons.imaging.formats.jpeg.JpegImageMetadata;
import org.apache.commons.imaging.formats.jpeg.exif.ExifRewriter;
import org.apache.commons.imaging.formats.tiff.write.TiffOutputSet;
import org.apache.commons.lang3.tuple.ImmutableTriple;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import static android.app.Activity.RESULT_CANCELED;
import static android.app.Activity.RESULT_OK;
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
        galleryPickerIntent.setType("image/*");
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

            boolean preserveMetadata = true;
            if (options.hasKey("preserveMetadata"))
                preserveMetadata = options.getBoolean("preserveMetadata");

            TiffOutputSet originalImageMetaData = null;
            if (preserveMetadata && format.equals(Bitmap.CompressFormat.JPEG)) {
                try {
                    String realFilePath = contentResolver.resolveUri(imageUri);//looks like: file:/some-path/blah
                    File file = new File(Uri.parse(realFilePath).getPath());
                    JpegImageMetadata jpegImageMetadata = (JpegImageMetadata) Imaging.getMetadata(file);
                    originalImageMetaData = jpegImageMetadata.getExif().getOutputSet();
                } catch (Exception e) {
                    Log.w(TAG, "failed to read image metadata", e);
                }
            }

            editorListener.add(promise, originalImageMetaData);

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

            if (!saveTo.equals("photos"))
                builder.withOutput(File.createTempFile("rnimagetools.", "." + format.name(), reactContext.getCacheDir()));

            getReactApplicationContext().startActivityForResult(builder.build(), REQ_CODE_CSDK_IMAGE_EDITOR, null);
        } catch (Exception e) {
            promise.reject("error", "failed to start editor", e);
        }
    }

    private abstract class SelectImageListener extends BaseActivityEventListener {
        private final List<Promise> callbacks = new ArrayList<>();
        private final int requestCode;

        public SelectImageListener(int requestCode) {
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
                    resolve(uri.toString());
                } else {
                    reject("no output uri");
                }
            } else {
                reject("unrecognised return code: " + resultCode);
            }
        }

        void resolve(String realPathFromURI) {
            while (!callbacks.isEmpty()) {
                callbacks.remove(0).resolve(realPathFromURI);
            }
        }

        void reject(String reason) {
            while (!callbacks.isEmpty()) {
                callbacks.remove(0).reject("error", reason);
            }
        }

        protected abstract Uri uriFrom(Intent data);

        public void add(Promise promise) {
            callbacks.add(promise);
        }
    }

    private class GalleryListener extends SelectImageListener {
        public GalleryListener(int code) {
            super(code);
        }

        @Override
        protected Uri uriFrom(Intent data) {
            return data.getData();
        }
    }

    private class EditorListener extends SelectImageListener {
        private TiffOutputSet originalImageMetadata;

        public EditorListener(int code) {
            super(code);
        }

        @Override
        void resolve(String editorOutputPath) {
            if (this.originalImageMetadata != null) {
                try {
                    //todo: deal with width, height and orientation metadata as they may not be correct after being edited
                    ImageMetadataTools imageMetadataAfter = ImageMetadataTools.createImageMetadata(contentResolver.readBytes(editorOutputPath));
                    WritableMap metadataAfter = imageMetadataAfter.asMap();

                    File editorOutputFile = new File(contentResolver.resolveUri(Uri.parse(editorOutputPath)));
                    final File outputFileWithMetadata = new File(editorOutputFile.getParentFile(), UUID.randomUUID() + "_" + editorOutputFile.getName());

                    FileOutputStream output = new FileOutputStream(outputFileWithMetadata);
                    try {
                        new ExifRewriter()
                                .updateExifMetadataLossy(editorOutputFile, output, originalImageMetadata);
                    } finally {
                        output.close();
                    }

                    MediaScannerConnection.scanFile(reactContext, new String[]{outputFileWithMetadata.getAbsolutePath()}, null, new MediaScannerConnection.OnScanCompletedListener() {
                        @Override
                        public void onScanCompleted(String path, Uri uri) {
                            EditorListener.super.resolve(outputFileWithMetadata.getAbsolutePath());
                        }
                    });
                } catch (ImageReadException | IOException | ImageWriteException e) {
                    Log.e("RNImageTools", "failed to copy original image metadata", e);
                }
            }

            super.resolve(editorOutputPath);
        }

        @Override
        protected Uri uriFrom(Intent data) {
            return data.getParcelableExtra(AdobeImageIntent.EXTRA_OUTPUT_URI);
        }

        public void add(Promise promise, TiffOutputSet originalImageMetadata) {
            this.originalImageMetadata = originalImageMetadata;
            super.add(promise);
        }
    }

}