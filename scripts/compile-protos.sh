#!/bin/sh

protoc --grpc_out lib/rpc/grpc --cpp_out lib/rpc/grpc -I protos --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) protos/helloworld.proto
