import javax.crypto.Cipher;
import java.io.BufferedReader;
import java.io.FileReader;
import java.lang.reflect.Array;
import java.nio.charset.StandardCharsets;
import java.security.Key;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.MessageDigest;
import java.util.Arrays;
import java.util.Base64;


public class DigitalSignatureSolution {
    public static String byteToHexString(byte[] input) {
        StringBuilder output = new StringBuilder();
        for (byte b : input) {
            output.append(String.format("%02X", b));
        }
        return output.toString();
    }

    public static void main(String[] args) throws Exception {
//Read the text file and save to String data
        String fileName = "src/shorttext.txt";
//        String fileName = "src/longtext.txt";
        String data = "";
        String line;
        BufferedReader bufferedReader = new BufferedReader(new FileReader(fileName));
        while ((line = bufferedReader.readLine()) != null) {
            data = data + "\n" + line;
        }
        System.out.println("Original content: " + data);

//TODO: generate a RSA keypair, initialize as 1024 bits, get public key and private key from this keypair.
        KeyPairGenerator keyGen = KeyPairGenerator.getInstance("RSA");
        KeyPair keyPair = keyGen.generateKeyPair();
        Key publicKey = keyPair.getPublic();
        Key privateKey = keyPair.getPrivate();

//TODO: Calculate message digest, using MD5 hash function
        MessageDigest md = MessageDigest.getInstance("MD5");
        md.update(data.getBytes(StandardCharsets.UTF_8));
        byte[] digest = md.digest();

//TODO: print the length of output digest byte[], compare the length of file smallSize.txt and largeSize.txt
        System.out.println("byteToHexString(digest): " + byteToHexString(digest));
        System.out.println("digest.length: " + digest.length);
        // small: 5044643FCA56CF59FD8143DFFA5E13C9, length = 16
        // large: A07D79C37A85635C67C8D13467CFA44D, length = 16

//TODO: Create RSA("RSA/ECB/PKCS1Padding") cipher object and initialize is as encrypt mode, use PRIVATE key.
        Cipher rsaCipher = Cipher.getInstance("RSA/ECB/PKCS1Padding");
        rsaCipher.init(Cipher.ENCRYPT_MODE, privateKey);

//TODO: encrypt digest message
        byte[] signedDigest = rsaCipher.doFinal(digest);

//TODO: print the encrypted message (in base64format String using DatatypeConverter)
        // not able to use DatatypeConverter, removed since
        String signedDigestHex = byteToHexString(signedDigest);
        System.out.println("signedDigestHex: " + signedDigestHex);
        System.out.println("signedDigest.length: " + signedDigest.length);

//TODO: Create RSA("RSA/ECB/PKCS1Padding") cipher object and initialize is as decrypt mode, use PUBLIC key.           
        Cipher desCipher = Cipher.getInstance("RSA/ECB/PKCS1Padding");
        desCipher.init(Cipher.DECRYPT_MODE, publicKey);

//TODO: decrypt message
        byte[] decryptedSignedDigest = desCipher.doFinal(signedDigest);
        System.out.println("decryptedSignedDigest.length: " + decryptedSignedDigest.length);

//TODO: print the decrypted message (in base64format String using DatatypeConverter), compare with origin digest
        String decryptedSignedDigestHex = byteToHexString(decryptedSignedDigest);
        System.out.println("decryptedSignedDigestHex: " + decryptedSignedDigestHex);
        System.out.println("decryptedSignedDigestHex.length(): " + decryptedSignedDigestHex.length());
        System.out.println("Arrays.equals(digest, decryptedSignedDigest): " + Arrays.equals(digest, decryptedSignedDigest));
    }

}