package tettoon.pipara;

import java.io.IOException;

public class PiParaCtl {

    public static void main(String[] args) {
        if (args.length < 1) {
            System.err.println("An argument required.");
            System.exit(1);
        }

        int data = 0;
        try {
            data = Integer.parseInt(args[0], 16);
        } catch (NumberFormatException e) {
            e.printStackTrace(System.err);
            System.err.println("Invalid argument.");
            System.exit(2);
        }

        try {
            PiParaJni pipara = new PiParaJni();
            pipara.write((byte) data);
        } catch (IOException e) {
            e.printStackTrace(System.err);
            System.exit(3);
        }
    }

}

