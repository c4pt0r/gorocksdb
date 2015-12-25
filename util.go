package gorocksdb

import "C"

import (
	"bytes"
	"io"
	"reflect"
	"unsafe"
)

// btoi converts a bool value to int
func btoi(b bool) int {
	if b {
		return 1
	}
	return 0
}

// boolToChar converts a bool value to C.uchar
func boolToChar(b bool) C.uchar {
	if b {
		return 1
	}
	return 0
}

func ucharToByte(data *C.uchar, len C.size_t) []byte {
	var value []byte

	sH := (*reflect.SliceHeader)(unsafe.Pointer(&value))
	sH.Cap, sH.Len, sH.Data = int(len), int(len), uintptr(unsafe.Pointer(data))

	return value
}

func charToByte(data *C.char, len C.size_t) []byte {
	var value []byte

	sH := (*reflect.SliceHeader)(unsafe.Pointer(&value))
	sH.Cap, sH.Len, sH.Data = int(len), int(len), uintptr(unsafe.Pointer(data))

	return value
}

// byteToChar returns *C.char from byte slice
func byteToChar(b []byte) *C.char {
	var c *C.char
	if len(b) > 0 {
		c = (*C.char)(unsafe.Pointer(&b[0]))
	}

	return c
}

// byteToUChar returns *C.char from byte slice
func byteToUChar(b []byte) *C.uchar {
	var c *C.uchar
	if len(b) > 0 {
		c = (*C.uchar)(unsafe.Pointer(&b[0]))
	}

	return c
}

// stringToChar returns *C.char from string
func stringToChar(s string) *C.char {
	ptrStr := (*reflect.StringHeader)(unsafe.Pointer(&s))

	return (*C.char)(unsafe.Pointer(ptrStr.Data))
}

// charSlice converts a C array of *char to a []*C.char.
func charSlice(data **C.char, len C.int) []*C.char {
	var value []*C.char

	sH := (*reflect.SliceHeader)(unsafe.Pointer(&value))
	sH.Cap, sH.Len, sH.Data = int(len), int(len), uintptr(unsafe.Pointer(data))

	return value
}

// sizeSlice converts a C array of size_t to a []C.size_t.
func sizeSlice(data *C.size_t, len C.int) []C.size_t {
	var value []C.size_t

	sH := (*reflect.SliceHeader)(unsafe.Pointer(&value))
	sH.Cap, sH.Len, sH.Data = int(len), int(len), uintptr(unsafe.Pointer(data))

	return value
}

func appendUint32AsByte(b []byte, v uint32) []byte {
	b = append(b, byte(v))
	b = append(b, byte(v>>8))
	b = append(b, byte(v>>16))
	b = append(b, byte(v>>24))
	return b
}

func bytesToUint32(b []byte) uint32 {
	return uint32(uint32(b[0]) | uint32(b[1])<<8 | uint32(b[2])<<16 | uint32(b[3])<<24)
}

func unpackBuflist(b []byte) [][]byte {
	var ret [][]byte
	rdr := bytes.NewBuffer(b)
	head := make([]byte, 4)
	for {
		_, err := rdr.Read(head)
		if err == io.EOF {
			break
		}
		if err != nil {
			panic(err)
		}
		n := bytesToUint32(head)
		buf := make([]byte, n)
		_, err = rdr.Read(buf)
		if err != nil {
			panic(err)
		}
		ret = append(ret, buf)
	}
	return ret
}

func appendBuf(b []byte, node []byte) []byte {
	ret := appendUint32AsByte(b, uint32(len(node)))
	ret = append(ret, node...)
	return ret
}
