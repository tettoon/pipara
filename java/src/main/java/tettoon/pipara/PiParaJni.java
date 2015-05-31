package tettoon.pipara;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;

public class PiParaJni {

    private static boolean libraryLoaded;

    private static Path tempNativeLibrary;

    private static Path tempJniLibrary;

    static {
        if (isSupported()) {
            try {
                tempNativeLibrary = copyLibrary("libpipara.so", "libpipara");
                System.load(tempNativeLibrary.toAbsolutePath().toString());
                tempJniLibrary = copyLibrary("libpiparajni.so", "libpiparajni");
                System.load(tempJniLibrary.toAbsolutePath().toString());
                libraryLoaded = true;
            } catch (IOException e) {
                e.printStackTrace(System.err);
            } catch (UnsatisfiedLinkError e) {
                e.printStackTrace(System.err);
            }
        }
    }

    private static Path copyLibrary(String name, String prefix) throws IOException {
        try (InputStream is = PiParaJni.class.getResourceAsStream(name)) {
            Path temp = Files.createTempFile(prefix, ".so");
            Files.copy(is, temp, StandardCopyOption.REPLACE_EXISTING);
            temp.toFile().deleteOnExit();
            return temp;
        }
    }

    public static boolean isLinux() {
        String osName = System.getProperty("os.name").toLowerCase();
        return "linux".equals(osName);
    }

    public static boolean isArm() {
        String osArch = System.getProperty("os.arch").toLowerCase();
        return "arm".equals(osArch);
    }

    public static boolean isSupported() {
        return isArm() && isLinux();
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

