import java.net.InetAddress;
import java.net.UnknownHostException;

public class DnsJava3 {
    public static void main(String[] args) throws UnknownHostException, InterruptedException {
        String addrArg = args[0];

        while (true) {
            InetAddress[] addresses = InetAddress.getAllByName(addrArg);
            Thread.sleep(100);
        }
    }
}
