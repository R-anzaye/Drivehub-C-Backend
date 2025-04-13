#!/bin/bash

# Define URLs for the latest Mongoose files
MONGOOSE_C_URL="https://raw.githubusercontent.com/cesanta/mongoose/master/mongoose.c"
MONGOOSE_H_URL="https://raw.githubusercontent.com/cesanta/mongoose/master/mongoose.h"

# Destination directory
DEST_DIR="./mongoose"

# Create the directory if it doesn't exist
mkdir -p "$DEST_DIR"

echo "Downloading latest mongoose.c..."
curl -fsSL "$MONGOOSE_C_URL" -o "$DEST_DIR/mongoose.c"

echo "Downloading latest mongoose.h..."
curl -fsSL "$MONGOOSE_H_URL" -o "$DEST_DIR/mongoose.h"

echo "✅ Mongoose has been updated successfully in $DEST_DIR."
