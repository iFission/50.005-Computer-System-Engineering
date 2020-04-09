import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import javax.crypto.*;
import java.util.Base64;


public class DesSolution {
    public static void main(String[] args) throws Exception {
        String fileName = "src/shorttext.txt";
//        String fileName = "src/longtext.txt";
        String data = "";
        String line;
        BufferedReader bufferedReader = new BufferedReader(new FileReader(fileName));
        while ((line = bufferedReader.readLine()) != null) {
            data = data + "\n" + line;
        }
        System.out.println("Original content: " + data);

//TODO: generate secret key using DES algorithm
        KeyGenerator keyGen = KeyGenerator.getInstance("DES");
        SecretKey desKey = keyGen.generateKey();

//TODO: create cipher object, initialize the ciphers with the given key, choose encryption mode as DES
        Cipher desCipher = Cipher.getInstance("DES/ECB/PKCS5Padding");
        desCipher.init(Cipher.ENCRYPT_MODE, desKey);

//TODO: do encryption, by calling method Cipher.doFinal().
        byte[] dataByteArray = data.getBytes();
        byte[] encryptedBytesArray = desCipher.doFinal(dataByteArray);

//TODO: print the length of output encrypted byte[], compare the length of file smallSize.txt and largeSize.txt
        System.out.println(encryptedBytesArray.length);

//TODO: do format conversion. Turn the encrypted byte[] format into base64format String using DatatypeConverter
        String dataEncrypted =
                Base64.getEncoder().encodeToString(encryptedBytesArray);

//TODO: print the encrypted message (in base64format String format)
        System.out.println(dataEncrypted);

//TODO: create cipher object, initialize the ciphers with the given key, choose decryption mode as DES
        desCipher.init(Cipher.DECRYPT_MODE, desKey);

//TODO: do decryption, by calling method Cipher.doFinal().
        byte[] decryptedBytesArray = desCipher.doFinal(encryptedBytesArray);

//TODO: do format conversion. Convert the decrypted byte[] to String, using "String a = new String(byte_array);"
        String decryptedData = new String(decryptedBytesArray);

//TODO: print the decrypted String text and compare it with original text
        System.out.println(decryptedData);
        System.out.println(encryptedBytesArray);
        System.out.println(encryptedBytesArray.length);

    }
}