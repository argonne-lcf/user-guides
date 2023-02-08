# TODO

- [ ] Simple how to run data parallel on SN.
- [X] On first glance, It would be good to include information of what model is being run here.
- [X] Can users just take an existing compiled model and run? No, they need to recompile here for this, right? Users coming from the GPU land would falter here otherwise.
- [X] How is data parallel implemented in a single sentence would be good.
- [ ] What does intelligently split data mean? Is there a special or particular way it is done or can be done that is different from what is done, say, on a GPU or other systems today?
- [ ] What happens if I set ws = 3
- [X] Why are we setting OMP_NUM_THREADS=8? What happens if we set it lower. If this is important, we should highlight it
- [X] Mention that MPI is used to do data-parallel training. Number of MPI ranks should be equal to RDUs requested
- [X] What does reduce on rdu imply. Please provide more details here.
- [ ] How does someone who has worked with Horovod or DDP move to use data parallel on SN?
- [ ] Does bs=1 mean local batch.

- [ ] docs/sambanova/Best-Practices-and-FAQs.md ## MPI -- TODO -- this needs to be redone - may be part of data parallel page.
- [ ] Edit Anatomy... to be a technical doc
- [ ] Edit DataParallel.md to be a technical doc
- [...] SN provide indexing for their docs. From training
- [X] We need to include in the documentation where the SN docs are located,
- [X] what the contents are,
- [X] and how to view them.
- [X] As part of what the contents are, we need some readme.md with the PDFs listed
- [X] SN PDF docs copy them locally. and then view them.  VV has a tar file also.

- [X] Move snconfig from overview to Miscellaneous
- [X] Ask SN for a diagram.
- [X] Use SN diagram.

```text
## [SambaNova](https://github.com/argonne-lcf/ai-testbed-userdocs/tree/main/docs/sambanova)
**DONE**1. [System Overview](https://github.com/argonne-lcf/ai-testbed-userdocs/blob/main/docs/sambanova/System-Overview.md)
    - **DONE** "half-rack system" ? **Yes**
    - **DONE**Configuration section says
        > SambaNova node can be accessed as sm-01.ai.alcf.anl.gov. **This is the actual SN node.**
        
       but page on [How to setup your base environment](https://github.com/argonne-lcf/ai-testbed-userdocs/blob/main/docs/sambanova/How-to-setup-your-base-environment.md)says
        ```bash
        ssh ALCFUserID@sambanova.alcf.anl.gov
        ```
        **This is the log in node.**
    
2. [How to setup your base environment](https://github.com/argonne-lcf/ai-testbed-userdocs/blob/main/docs/sambanova/System-Overview.md)
    **DONE**1. Page title reads like an instruction, whereas most of the others read like section titles **docs/sambanova/Logging-into-a-SambaNova-Node.md**
    **SKIPPED**2. Could combine this page with [Using virtual environments to customize environment](https://github.com/argonne-lcf/ai-testbed-userdocs/blob/main/docs/sambanova/Using-virtual-environments-to-customize-environment.md) into a single **Environment Setup** page??

3. [Using virtual environments to customize environment](https://github.com/argonne-lcf/ai-testbed-userdocs/blob/main/docs/sambanova/Using-virtual-environments-to-customize-environment.md)
    **DONE** **As script**1. Might be worth mentioning what packages are included in `--system-site-packages`
    2. What commonly used packages are missing / might need to be installed manually?
4. [Steps to run a model / program](https://github.com/argonne-lcf/ai-testbed-userdocs/blob/main/docs/sambanova/Steps-to-run-a-model-or-program.md)
    - Might be worth mentioning that the [Example Programs](https://github.com/argonne-lcf/ai-testbed-userdocs/blob/main/docs/sambanova/Example-Programs.md) page has instructions for making a local copy of the examples used in the documentation
    - "The SambaNova workflow includes the following main steps to run a model"
        - Maybe list steps explicitly and then expand on them in their respective sections? e .g.
        - The SambaNova workflow includes the following main steps to run a model:
            1. Compile
            2. Run
            3. Test (optional)
        - Seems a bit unnecessary to list them as steps if the Test step is optional?
    - **DONE**Where is the `pef` directory coming from in the `srun python lenet.py run --pef="pef/lenet/lenet.pef `command?


11. [Miscellaneous](https://github.com/argonne-lcf/ai-testbed-userdocs/blob/main/docs/sambanova/Miscellaneous.md)
    **VV seems fine with the original**1. Might be a good idea to include the links under Resources somewhere on the main page?
12. 

- [ ] [Anatomy of SambaNova PyTorch Models](https://github.com/argonne-lcf/ai-testbed-userdocs/blob/main/docs/sambanova/Anatomy-of-SambaNova-PyTorch-Models.md)
    1. Updates section references https://git.cels.anl.gov/ai-testbed-apps/sambanova/sambanova_starter.git but that link requires special permission to view


- [X] **There is already a link to the Example Programs.**Page Steps to Run a Model/Program : Add “ cd ~/apps/starters/“ before the example 
- [X] **I would have to set up a new repo from SC22 paper.** Page Performance Tools : Measure TFLOPs : Conv2D forward pass example file location can be provided
- [X] Page Example Programs : section UNet : Remove question mark here --batch-size=1? python unet.py run --do-train  --in-channels=3  --in-width=32  --in-height=32 --init-features 32 --batch-size=1? --data-dir $DATADIR --log-dir ${OUTDIR}/log_dir_unet32_train --epochs 5 --pef=${OUTDIR}/unet_train/unet_train.pef

Page Best Practices and FAQs : Section Data Parallel : Add unet_main.py file location
- [X] Page Best Practices and FAQs : Section Data Parallel : Remove “- —“ near python unet_main.py compile - --batch-size=48
- [X] Page Best Practices and FAQs : Section Data Parallel : Remove “- —“  unet_main.py run - -p . 
- [Doc'd elsewhere] Also set export - [ SOFTWARE_HOME before compile and add to —pef-name 
- [X] Argonne SambaNova-Training-June2021 and Human Decisions Files notes link is broken 

Typos : Page :Anatomy of SambaNova PyTorch Model  typo at
- [X] “Then once every 10,000 epics, print the status”
- [X] “Contains the train and test methods and a littile more.”
- [X] “instantiated instantiate”
- [X] “the the bigger systems “
- [X] “t the the various “

Page Best Practices and FAQs: 
- [UTL] “t and environment variable” 
- [X] “where the loss is calculating across - calculated“ 

- [X] Page Steps to Run a Model/Program: “Test (Optional) This commands - command“
- [X] Page How to Setup Your Base Environment - “ request an acccount a” “generted - generated” 


- [X] ModelZoo to /software/sambanova/apps/ and also use symbolic link for latest.

- [X] How to do DataParallel.  add page.  See Confluence and maybe video and its .pptx

- [X] How to spec 2 or more RDUs on SN?  Queue and Sub also.  
- [model parallel] can do in compile phase v. run can be different.

- [X] Move SN starter code to /software/sambanova/tutorials

- [OK] just fyi, here's how to serve html from the bastion node (e.g. cerebras.alcf.anl.gov):

```bash
ssh -t -L localhost:8089:localhost:8089 arnoldw@cerebras.alcf.anl.gov  "cd /software/cerebras/docs/V1.1/;python3 -m http.server 8089"
```

## System Overview

- [ ] First draft
- [ ] First draft reviewed
- [ ] Second draft
- [ ] Second draft reviewed
- [ ] Final draft
- [ ] Final draft reviewed

## How to setup your base environment

- [X] First draft
- [X] First draft reviewed
- [X] Second draft 4/18
- [X] Second draft reviewed
- [X] Third draft 4/20
- [ ] Third draft reviewed
- [ ] Final draft
- [ ] Final draft reviewed

## Using Virtual Environments to Customize Environment

- [X] First draft
- [X] First draft reviewed
- [X] Second draft 4/18
- [X] Second draft reviewed
- [X] Third draft
- [ ] Third draft reviewed
- [ ] Final draft
- [ ] Final draft reviewed

## Steps to Run a Model/Program

- [X] First draft
- [X] First draft reviewed
- [X] Second draft 4/19
- [X] Second draft reviewed
- [X] Third draft
- [ ] Third draft reviewed
- [ ] Final draft
- [ ] Final draft reviewed

## Job Queueing and Submission

- [X] First draft
- [X] First draft reviewed
- [X] Second draft 4/18
- [X] Second draft reviewed
- [X] Third draft
- [ ] Third draft reviewed
- [ ] Final draft
- [ ] Final draft reviewed

## Example Programs

- [X] First draft
- [X] First draft reviewed
- [X] Second draft 4/19
- [X] Second draft reviewed
- [X] Third draft
- [ ] Third draft reviewed
- [ ] Final draft
- [ ] Final draft reviewed

## Performance Tools

- [X] First draft 4/19
- [X] First draft reviewed
- [ ] Second draft
- [ ] Second draft reviewed
- [ ] Third draft
- [ ] Third draft reviewed
- [ ] Final draft
- [ ] Final draft reviewed

## Best Practices and FAQs

- [X] First draft 4/19
- [X] First draft reviewed
- [ ] Second draft
- [ ] Second draft reviewed
- [ ] Third draft
- [ ] Third draft reviewed
- [ ] Final draft
- [ ] Final draft reviewed

## Tunneling and Forwarding Ports

- [X] First draft 4/19
- [X] First draft reviewed
- [ ] Second draft
- [ ] Second draft reviewed
- [ ] Third draft
- [ ] Third draft reviewed
- [ ] Final draft
- [ ] Final draft reviewed

## System and Storage Policies

- [ ] First draft
- [ ] First draft reviewed
- [ ] Second draft
- [ ] Second draft reviewed
- [ ] Third draft
- [ ] Third draft reviewed
- [ ] Final draft
- [ ] Final draft reviewed

## Miscellaneous

- [X] First draft 4/19
- [X] First draft reviewed
- [ ] Second draft
- [ ] Second draft reviewed
- [ ] Third draft
- [ ] Third draft reviewed
- [ ] Final draft
- [ ] Final draft reviewed

## Anatomy of SambaNova PyTorch Models

- [X] First draft
- [X] First draft reviewed
- [ ] Second draft
- [ ] Second draft reviewed
- [ ] Third draft
- [ ] Third draft reviewed
- [ ] Final draft
- [ ] Final draft reviewed
