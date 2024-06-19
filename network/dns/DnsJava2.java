import java.net.InetAddress;
import java.util.concurrent.CountDownLatch;

public class DnsJava2 {
    public static void main(String[] args) throws InterruptedException {
        String addrArg = args[0];

        int n = 10;
        CountDownLatch latch = new CountDownLatch(n);
        for (int i = 0; i < n; i++) {
            new Thread(() -> {
                try {
                    InetAddress[] addresses = InetAddress.getAllByName(addrArg);
                    for (InetAddress addr : addresses) {
                        System.out.println(addr);
                    }
                } catch (Exception e) {
                    System.out.println("error: " + e);
                } finally {
                    latch.countDown();
                }
            }).start();
        }
        latch.await();
        System.out.println("-----------");
    }
}

