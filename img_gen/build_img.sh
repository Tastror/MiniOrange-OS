# if this get wrong, please change the CRLF to LF
# or you can try 

# 提交时转换为 LF，检出时不转换
# git config --global core.autocrlf input
# 提交包含混合换行符的文件时给出警告
# git config --global core.safecrlf warn

if [ $# -ne 4 ] ;then
        echo "usage: $0 \$(OBJDIR) \$(GENERATE_IMAGE_C) \$(OSBOOT_START_OFFSET)"
        exit 1
fi

IMAGE=$1
OBJDIR=$2
GENERATE_IMAGE_C=$3
OSBOOT_START_OFFSET=$4

gcc ${GENERATE_IMAGE_C} -o ${OBJDIR}/gen_img_executable
${OBJDIR}/gen_img_executable ${IMAGE}
dd if=${OBJDIR}/boot/mbr.bin of=${IMAGE} bs=1 count=446 conv=notrunc

loop_device=`losetup -f`
sudo losetup -P ${loop_device} ${IMAGE}
sudo mkfs.vfat -F 32 ${loop_device}p1
dd if=${OBJDIR}/boot/boot.bin of=${IMAGE} bs=1 count=420 seek=${OSBOOT_START_OFFSET} conv=notrunc

mkdir iso
sudo mount ${loop_device}p1 iso/
sudo cp ${OBJDIR}/boot/loader.bin iso/ -v
sudo cp ${OBJDIR}/kernel/kernel.bin iso/ -v
sudo umount iso/
rm -r iso

sudo losetup -d ${loop_device}