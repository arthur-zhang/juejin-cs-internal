string_asm_01:
	go tool compile -S -N -l string_asm_01.go | sed 's/\/data\/dev\/ya\/juejin_os_course\/basic\/go-asm\///g'
#string_asm_02:
	#GOOS=linux GOARCH=amd64  go build -gcflags "-N -l" -ldflags=-compressdwarf=false -o main.out string_asm_02.go
#string_asm_03:
	#GOOS=linux GOARCH=amd64  go build -gcflags "-N -l" -ldflags=-compressdwarf=false -o main.out string_asm_03.go
objdump:
	go tool objdump -s "main.*" main.out
