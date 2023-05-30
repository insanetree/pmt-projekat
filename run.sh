#!/bin/bash

mkdir -p "$NS3ROOT/scratch/projekat"

pushd src
for file in $(ls)
do
	cp $file "$NS3ROOT/scratch/projekat/$file"
done
popd

pushd h
for file in $(ls)
do
	cp $file "$NS3ROOT/scratch/projekat/$file"
done
popd

mkdir -p output
OUTDIR="$PWD/output"

pushd "$NS3ROOT"
./ns3 build
./ns3 run scratch/projekat.main.cc --cwd=$OUTDIR
popd
