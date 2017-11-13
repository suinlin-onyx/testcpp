package com.qq.myapplication;

import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.Spinner;
import android.widget.TextView;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = MainActivity.class.getSimpleName();
    private Button buttonLeft, buttonRight, buttonLeftVer, buttonRightVer;
    private TextView textViewLeft, textViewRight;
    private Spinner spinnerLeft, spinnerRight;
    private ScrollView scrollViewLeft, scrollViewRight;
    private Runnable javaRun, jniRun;

    private static final int SHOW_LINE = 3000;

    public enum AscDescOrder {
        Asc, Desc
    }

    public enum SortOrder {
        None, Name, Size, CreationTime, ModifyTime
    }


    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        buttonLeft = (Button) findViewById(R.id.button_left);
        buttonRight = (Button) findViewById(R.id.button_right);
        buttonLeftVer = (Button) findViewById(R.id.button_left_ver);
        buttonRightVer = (Button) findViewById(R.id.button_right_ver);

        textViewLeft = (TextView) findViewById(R.id.text_left);
        textViewRight = (TextView) findViewById(R.id.text_right);

        scrollViewLeft = (ScrollView) findViewById(R.id.scroll_view_left);
        scrollViewRight = (ScrollView) findViewById(R.id.scroll_view_right);

        textViewLeft.setMovementMethod(ScrollingMovementMethod.getInstance());
        textViewRight.setMovementMethod(ScrollingMovementMethod.getInstance());

        spinnerLeft = (Spinner) findViewById(R.id.spinner_sort);
        spinnerRight = (Spinner) findViewById(R.id.spinner_ado);

        buttonLeft.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                JniRun(null);
            }
        });
        buttonRight.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                JavaRun(null);
            }
        });
        buttonLeftVer.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                JniRun("a");
            }
        });
        buttonRightVer.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                JavaRun("a");
            }
        });
        initData();

    }

    public void onResume() {
        super.onResume();
    }

    private void initData() {

        final List<String> sorts = new ArrayList<>();
        for (SortOrder sortOrder : SortOrder.values()) {
            sorts.add(sortOrder.name());
        }
        spinnerLeft.setAdapter(new ArrayAdapter<String>(this, android.R.layout.simple_spinner_dropdown_item, sorts));


        final List<String> ado = new ArrayList<>();
        for (AscDescOrder ascDescOrder : AscDescOrder.values()) {
            ado.add(ascDescOrder.name());
        }
        spinnerRight.setAdapter(new ArrayAdapter<String>(this, android.R.layout.simple_spinner_dropdown_item, ado));

    }

    private SortOrder getSortBy() {
        int i = spinnerLeft.getSelectedItemPosition();
        String name = (String) spinnerLeft.getSelectedItem();
        for (SortOrder s : SortOrder.values()) {
            if (s.name().equals(name)) {
                return s;
            }
        }
        return SortOrder.Name;
    }

    private AscDescOrder getOrderBy() {
        int i = spinnerRight.getSelectedItemPosition();
        String name = (String) spinnerRight.getSelectedItem();
        for (AscDescOrder s : AscDescOrder.values()) {
            if (s.name().equals(name)) {
                return s;
            }
        }
        return AscDescOrder.Asc;
    }

    public void JniRun(String name) {
        if (jniRun == null) {
            jniRun = new JniRun(name, new CallbackRun() {
                @Override
                public void closeRun() {
                    MainActivity.this.jniRun = null;
                }
            });
            jniRun.run();
        }


    }

    public void JavaRun(String name) {
        if (javaRun == null) {
            javaRun = new JavaRun(name, new CallbackRun() {
                @Override
                public void closeRun() {
                    MainActivity.this.javaRun = null;
                }
            });
            javaRun.run();
        }
    }


    long startTime = 0;
    Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            int what = msg.what;
            int restart = msg.arg1;
            switch (what) {
                case 1:
                    String name = (String) msg.obj;
                    if (textViewLeft != null && name != null) {
                        StringBuffer stringBuffer = new StringBuffer();
                        if (restart > 0) {
                            if (restart == 1) {
                                startTime = System.currentTimeMillis();
                            }
                            /*String text = textViewLeft.getText().toString();
                            if (text != null) {
                                stringBuffer.append(text).append("\n");
                            }*/

                        }

                        stringBuffer.append(name);
                        textViewLeft.setText(stringBuffer.toString());
                        if (restart == 10) {
                            Log.d(TAG, "更新UI耗时: " + (System.currentTimeMillis() - startTime) + "ms");
                        }
                    }

                    if (scrollViewLeft != null && restart == 0) {
                        scrollViewLeft.fullScroll(ScrollView.FOCUS_UP);
                    }
                    break;
                case 2:
                    name = (String) msg.obj;
                    if (textViewRight != null && name != null) {
                        StringBuffer stringBuffer = new StringBuffer();
                       /* String text = textViewRight.getText().toString();
                        if (text != null) {
                            stringBuffer.append(text).append("\n");
                        }*/

                        stringBuffer.append(name);
                        textViewRight.setText(stringBuffer.toString());
                    }

                    if (scrollViewRight != null) {
                        scrollViewRight.fullScroll(ScrollView.FOCUS_UP);
                    }
                    break;
            }
        }
    };

    interface CallbackRun {
        public void closeRun();
    }

    private String getPath() {
        String dir = "/books";
        StringBuffer stringBuffer = new StringBuffer(Environment.getExternalStorageDirectory().toString()).append(dir);
        String strPath = stringBuffer.toString();
        Log.d(TAG, "getPath : " + strPath);
        File file = new File(strPath);
        if (file.exists() && file.isDirectory()) {

        } else {
            strPath = "/mnt/sdcard" + dir;
        }
        Log.d(TAG, "getPath : " + strPath);
        return strPath;
    }

    class JniRun implements Runnable {
        final String strPath = getPath();
        String name;
        boolean tag = true;
        CallbackRun mCallbackRun;
        String listOfFiles[];

        public JniRun() {

        }

        public JniRun(String name, CallbackRun mCallbackRun) {
            this.name = name;
            this.mCallbackRun = mCallbackRun;
        }

        @Override
        public void run() {

            while (tag) {

                StringBuffer stringBuffer = new StringBuffer("");
                if (name != null && name.length() > 0) {
                    SortOrder sort = getSortBy();
                    AscDescOrder ad = getOrderBy();

                    long nStart = System.currentTimeMillis();
                   // listOfFiles = getFilePaths(strPath, sort.name(), ad.name(), 0, 3500);
                    String[] strings = new File(strPath).list();
                    initFilePaths(strings);

                    String time = sort + "," + ad + " 排序用时: " + (System.currentTimeMillis() - nStart) + " ms" +
                            "; 读取文件数" + listOfFiles.length;
                    Log.d(TAG, time);
                    stringBuffer.append("***").append(time).append("\n");
                    updateTextView(1, 0, stringBuffer.toString());

                   nStart = System.currentTimeMillis();
                    for (String s : listOfFiles) {
                        File file = new File(s);
                        //file.getName();
                    }
                    time = "java读取文件信息用时: " + (System.currentTimeMillis() - nStart) + " ms";
                    Log.d(TAG, time);
                    stringBuffer.append("***").append(time).append("\n");
                    updateTextView(1, 1, stringBuffer.toString());


                    nStart = System.currentTimeMillis();
                    StringBuilder stringBuilder = new StringBuilder();
                    int i = 0;
                    for (String s : listOfFiles) {
                        if (i++ < SHOW_LINE)
                            stringBuilder.append("---").append(s).append("\n");
                    }
                    time = "加载排序列表用时: " + (System.currentTimeMillis() - nStart) + " ms";
                    Log.d(TAG, time);
                    stringBuffer.append("***").append(time).append("\n");
                    stringBuffer.append(stringBuilder);
                    updateTextView(1, 10, stringBuffer.toString());

                } else {

                    for (SortOrder sort : SortOrder.values()) {
                        for (AscDescOrder ad : AscDescOrder.values()) {
                            long nStart = System.currentTimeMillis();
                            listOfFiles = getFilePaths(strPath, sort.name(), ad.name(), -1, -1);
                            String time = sort + "," + ad +
                                    "; 耗时: " + (System.currentTimeMillis() - nStart) + " ms" +
                                    "; 读取文件数" + listOfFiles.length;
                            stringBuffer.append("***").append(time).append("\n");
                            int i = 0;
                            for (String s : listOfFiles) {
                                if (i++ < 20)
                                    stringBuffer.append("---").append(s).append("\n");
                            }
                        }
                    }
                }

                if (mCallbackRun != null) {
                    mCallbackRun.closeRun();
                }
                tag = false;
            }
        }
    }

    public void updateTextView(int what, int restart, String string) {
        Message msg = new Message();
        msg.what = what;
        msg.obj = string;
        msg.arg1 = restart;
        handler.sendMessage(msg);
    }

    class JavaRun implements Runnable {
        final String strPath = getPath();
        String name;
        boolean tag = true;
        CallbackRun mCallbackRun;
        List<File> listOfFiles;

        public JavaRun() {

        }

        public JavaRun(String name, CallbackRun mCallbackRun) {
            this.name = name;
            this.mCallbackRun = mCallbackRun;
        }

        @Override
        public void run() {
            while (tag) {

                StringBuffer stringBuffer = new StringBuffer("");
                if (name != null && name.length() > 0) {
                    SortOrder sort = getSortBy();
                    AscDescOrder ad = getOrderBy();

                    long nStart = System.currentTimeMillis();
                    listOfFiles = fileSystemAdapter(strPath, sort, ad);
                    String time = sort + "," + ad +
                            " 用时: " + (System.currentTimeMillis() - nStart) + " ms" +
                            "; 读取文件数" + listOfFiles.size();

                    stringBuffer.append("***").append(time).append("\n");
                    int i = 0;


                    nStart = System.currentTimeMillis();
                    StringBuilder stringBuilder = new StringBuilder();
                    for (File s : listOfFiles) {
                        if (i++ < SHOW_LINE)
                            stringBuilder.append("---").append(s.getName()).append("\n");
                    }
                    time = "加载排序列表用时: " + (System.currentTimeMillis() - nStart) + " ms";
                    stringBuffer.append("***").append(time).append("\n");
                    stringBuffer.append(stringBuilder);

                } else {

                    for (SortOrder sort : SortOrder.values()) {
                        for (AscDescOrder ad : AscDescOrder.values()) {

                            long nStart = System.currentTimeMillis();
                            listOfFiles = fileSystemAdapter(strPath, sort, ad);
                            String time = sort + "," + ad +
                                    " 用时: " + (System.currentTimeMillis() - nStart) + " ms" +
                                    "; 读取文件数" + listOfFiles.size();

                            stringBuffer.append("***").append(time).append("\n");
                            int i = 0;
                            for (File s : listOfFiles) {
                                if (i++ < 20)
                                    stringBuffer.append("---").append(s.getName()).append("\n");
                            }
                        }
                    }
                }
                //Log.d(TAG, stringBuffer.toString());
                Message msg = new Message();
                msg.what = 2;
                msg.obj = stringBuffer.toString();
                handler.sendMessage(msg);

                if (mCallbackRun != null) {
                    mCallbackRun.closeRun();
                }
                tag = false;
            }
        }
    }

    static public List<File> fileSystemAdapter(final String path, final SortOrder sortBy, final AscDescOrder ascDescOrder) {
        long startTime = System.currentTimeMillis();
        File[] lists = new File(path).listFiles();
        Log.d(TAG, "fileSystemAdapter, test: " + (System.currentTimeMillis() - startTime) + "ms");
        if (lists == null) {

            Log.d(TAG, "未扫描到文件.");
        }

        List<File> fileList = new ArrayList<File>();
        startTime = System.currentTimeMillis();
        for (File temp : lists) {
    /*        if (temp.isHidden()) {
                continue;
            }
            if (temp.isDirectory()) {
                fileList.add(temp);
            }
            if (temp.isFile()) {
                fileList.add(temp);
            }*/
            //temp.getName();
            String s = temp.toString();

            s = s.substring(s.lastIndexOf("/") + 1);

            Log.d(TAG, "fileSystemAdapter, file: " + s);

        }
        Log.d(TAG, "fileSystemAdapter, test2: " + (System.currentTimeMillis() - startTime) + "ms");

        switch (sortBy) {
            case Name:
                Collections.sort(fileList, new Comparator<File>() {

                    @Override
                    public int compare(File lhs, File rhs) {
                        int i = ComparatorUtils.booleanComparator(
                                lhs.isDirectory(), rhs.isDirectory(), AscDescOrder.Desc);
                        if (i == 0) {
                            return ComparatorUtils.stringComparator(lhs.getName(), rhs.getName(), ascDescOrder);
                        }
                        return i;
                    }
                });
                break;
            case CreationTime:
                //Todo:Java 6 and belows seems could only get file's last modified time,could not get creation time.
                //reference site:http://stackoverflow.com/questions/6885269/getting-date-time-of-creation-of-a-file
                Collections.sort(fileList, new Comparator<File>() {
                    @Override
                    public int compare(File lhs, File rhs) {
                        int i = ComparatorUtils.booleanComparator(
                                lhs.isDirectory(), rhs.isDirectory(), AscDescOrder.Desc);
                        if (i == 0) {
                            return ComparatorUtils.longComparator(lhs.lastModified(), rhs.lastModified(), ascDescOrder);
                        }
                        return i;
                    }
                });
                break;

            case Size:
                Collections.sort(fileList, new Comparator<File>() {
                    @Override
                    public int compare(File lhs, File rhs) {
                        int i = ComparatorUtils.booleanComparator(
                                lhs.isDirectory(), rhs.isDirectory(), AscDescOrder.Desc);
                        if (i == 0) {
                            return ComparatorUtils.longComparator(lhs.length(),
                                    rhs.length(), ascDescOrder);
                        }
                        return i;
                    }
                });
                break;
        }
        return fileList;
    }

    public native String[] getFilePaths(String filePath, String sortBy, String orderBy, int startOffset, int endOffset);
    public native Object[][] initFilePaths(String[] filePaths);
}
