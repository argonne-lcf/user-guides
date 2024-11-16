# To move data to your daos posix container

## using CP

```bash
cp 	/lus/flare/projects/CSC250STDM10_CNDA/kaushik/thundersvm/input_data/real-sim_M100000_K25000_S0.836	/tmp/${DAOS_POOL}/${DAOS_CONT} 
```

## using daos filesystem copy 

```bash
daos filesystem copy --src /lus/flare/projects/CSC250STDM10_CNDA/kaushik/thundersvm/input_data/real-sim_M100000_K25000_S0.836             	--dst daos://tmp/${DAOS_POOL}/${DAOS_CONT}   
```
You may have to replace the DAOS_POOL and DAOS_CONT label with its UUIDs. UUIDs can be copied from 

```bash
daos pool query ${DAOS_POOL}
daos container query     $DAOS_POOL_NAME  $DAOS_CONT_NAME 
```

## using mpifileutils distributed CP (DCP)

You can also use other mpifileutils binaraies. 

```bash
mpifileutils/bin> ls 
dbcast  dbz2  dchmod  dcmp  dcp  dcp1  ddup  dfilemaker1  dfind  dreln  drm  dstripe  dsync  dtar  dwalk
```

Ref: https://docs.daos.io/v2.4/testing/datamover/

