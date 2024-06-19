fn main() {
    println!("Hello, world!");
    let clock_id = nix::time::ClockId::CLOCK_MONOTONIC;
    {
        let result = nix::time::clock_gettime(clock_id);
        println!("{:?}", result);
    }
    let clock_id = nix::time::ClockId::CLOCK_MONOTONIC_COARSE;
    {
        // let clock_res = nix::time::clock_getres(clock_id).unwrap();
        // println!("clock_res: {:?}", clock_res);

        let result = nix::time::clock_gettime(clock_id);
        println!("{:?}", result);
    }
}
