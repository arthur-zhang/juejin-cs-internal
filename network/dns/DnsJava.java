import java.net.InetAddress;
import java.net.UnknownHostException;

public class DnsJava {
    public static void main(String[] args) throws UnknownHostException, InterruptedException {
        String addrArg = args[0];

        System.out.println("-----------");
        InetAddress[] addresses = InetAddress.getAllByName(addrArg);
        for (InetAddress addr : addresses) {
            System.out.println(addr);
        }
        System.out.println("-----------");
    }
}
