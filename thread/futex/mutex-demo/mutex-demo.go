package main

import (
	"fmt"
	"sync"
)

func main() {
	// 创建一个互斥锁
	var mutex sync.Mutex
	// 创建一个计数器
	counter := 0

	// 启动10个goroutine,每个goroutine将计数器加1000次
	var wg sync.WaitGroup
	for i := 0; i < 10; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for j := 0; j < 1000; j++ {
				// 锁定互斥锁
				mutex.Lock()
				// 对计数器执行加1操作
				counter++
				// 解锁互斥锁
				mutex.Unlock()
			}
		}()
	}

	// 等待所有goroutine完成
	wg.Wait()
	fmt.Println("Counter:", counter)
}
