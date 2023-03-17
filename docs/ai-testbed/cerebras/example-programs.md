# Example Programs

## Use a local copy of the model zoo
Make a local copy of the Cerebras **modelzoo** and **anl_shared** repository, if not previously done, as follows.

```bash
mkdir ~/R1.7.1
cp -r /srv/software/cerebras/model_zoo/modelzoo/ ~/R1.7.1/modelzoo
cp -r /srv/software/cerebras/model_zoo/anl_shared/ ~/R1.7.1/anl_shared
```


## UNet
An implementation of this: [U-Net: Convolutional Networks for Biomedical Image Segmentation](https://arxiv.org/pdf/1505.04597.pdf), Ronneberger et.  al 2015<br>
To run Unet with the <a href="https://www.kaggle.com/c/severstal-steel-defect-detection">Severstal: Steel Defect Detection</a> kaggle dataset, using a pre-downloaded copy of the dataset,

```console
cd ~/R1.5/modelzoo/unet/tf
#rm -r model_dir_unet_base_severstal
cp /srv/software/cerebras/dataset/severstal-steel-defect-detection/params_severstal_sharedds.yaml configs/params_severstal_sharedds.yaml
csrun_cpu python run.py --mode=train --compile_only --params configs/params_severstal_sharedds.yaml --model_dir model_dir_unet_base_severstal --cs_ip $CS_IP
csrun_wse python run.py --mode=train --params configs/params_severstal_sharedds.yaml --model_dir model_dir_unet_base_severstal --max_steps 2000 --cs_ip $CS_IP
```
## Bert
An implementation of this: [BERT: Pre-training of Deep Bidirectional Transformers for Language Understanding](https://arxiv.org/abs/1810.04805)<br>
This BERT-large msl128 example uses a single sample dataset for both training and evaluation. See the README.md in the source directory for details on how to build a dataset from text input.
```console
cd ~/R1.5/modelzoo/transformers/tf/bert
cp /srv/software/cerebras/dataset/bert_large/params_bert_large_msl128_sampleds.yaml configs/params_bert_large_msl128_sampleds.yaml
#rm -r model_dir_bert_large_msl128
csrun_cpu python run.py --mode=train --compile_only --params configs/params_bert_large_msl128_sampleds.yaml --model_dir model_dir_bert_large_msl128 --cs_ip $CS_IP
csrun_wse python run.py --mode=train --params configs/params_bert_large_msl128_sampleds.yaml --model_dir model_dir_bert_large_msl128 --cs_ip $CS_IP
```

## BraggNN
An implementation of this: [BraggNN: fast X-ray Bragg peak analysis using deep
learning](https://journals.iucr.org/m/issues/2022/01/00/fs5198/fs5198.pdf)<br>
The BraggNN model has two versions:<br>
1) Convolution only - this version does not include the non-local attention block<br>
2) Nonlocal - This version includes the nonlocal attention block as described in  <br>
[https://arxiv.org/pdf/1711.07971.pdf](https://arxiv.org/pdf/1711.07971.pdf)

```console
cd ~/R1.5/anl_shared/braggnn/tf
cp /srv/software/cerebras/dataset/BraggN/params_bragg_nonlocal_sampleds.yaml configs/params_bragg_nonlocal_sampleds.yaml
#rm -r model_dir_braggnn
csrun_cpu python run.py -p configs/params_bragg_nonlocal_sampleds.yaml --model_dir model_dir_braggnn --mode train --compile_only --multireplica --cs_ip $CS_IP
csrun_wse python run.py -p configs/params_bragg_nonlocal_sampleds.yaml --model_dir model_dir_braggnn --mode train --multireplica --cs_ip $CS_IP
```

