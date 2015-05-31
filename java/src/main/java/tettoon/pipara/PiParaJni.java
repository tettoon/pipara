package tettoon.pipara;

import java.io.IOException;

public class PiParaJni {

    private static boolean libraryLoaded;

    static {
        if (isSupported()) {
            try {
                System.loadLibrary("piparajni");
                libraryLoaded = true;
            } catch (UnsatisfiedLinkError e) {
                e.printStackTrace(System.err);
            }
        }
    }

    public static boolean isLinux() {
        String osName = System.getProperty("os.name");
        return "liunx".equalsIgnoreCase(osName);
    }

    public static boolean isArm() {
        String osArch = System.getProperty("os.arch");
        return "arm".equalsIgnoreCase(osArch);
    }

    public static boolean isSupported() {
        return (isLinux() && isArm());
    }

    public PiParaJni() throws IOException {
        if (!isSupported()) {
            throw new IOException("Unsupported platform.");
        }
        if (!libraryLoaded) {
            throw new IOException("A JNI library has not been loaded.");
        }
        setup();
    }

    public native void setup() throws IOException;

    public native void write(byte data);

}

