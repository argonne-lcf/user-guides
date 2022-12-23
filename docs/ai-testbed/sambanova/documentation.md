# Documentation

The SambaNova documentation is housed at `/software/sambanova/docs/latest/` accessible via login node.

```text
getting-started.pdf             # Getting Started with SambaFlow
accuracy-debugging-tools.pdf    # Introduction to the model accuracy debugging tools.
compiler-options.pdf            # Provides advanced compiler options for the compile command.
conversion-to-sambaflow.pdf     # Converting existing models to SambaFlow
intermediate-tutorial.pdf       # SambaFlow intermediate model
intro-tutorial-pytorch.pdf      # A peek into the code of the above example program.
release-notes.pdf               # Provide new feature and bug fixes updates for each release version.
run-examples-language.pdf       # Run BERT on RDU
run-examples-pytorch.pdf        # Run Power PCA and micro models like GEMM on RDU
run-examples-vision.pdf         # Run UNET on RDU.
using-layernorm.pdf             # Example to use LayerNorm instead of BatchNorm
using-venvs.pdf                 # Python Virtual Environment.

latest\
    accuracy-debugging-tools.pdf
    compiler-options.pdf
    conversion-to-sambaflow.pdf
    getting-started.pdf
    intermediate-tutorial.pdf
    intro-tutorial-pytorch.pdf
    release-notes.pdf
    run-examples-language.pdf
    run-examples-pytorch.pdf
    run-examples-vision.pdf
    using-layernorm.pdf
    using-venvs.pdf
```

The documentation can be viewed on your local system by copying the files from the login node.

```bash
cd <your docs destination>
scp -r ALCFUserID@sambanova.alcf.anl.gov:/software/sambanova/docs/latest/* .
```

View the PDFs in your favorite viewer or web browser on your local machine.
