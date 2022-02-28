#!/usr/bin/env python3

import os
import glob
import shutil

# training data ratio
training_data = 0.8

# split the data into two sets: training and test  
def split_data(data_dir):

    # make expected directories
    os.system('mkdir -p ' + data_dir + 'training')
    os.system('mkdir -p ' + data_dir + 'test')
    
    # counter to keep track of splitting
    split_counter = 0
    
    # fetch all data filenames
    filepaths = glob.glob(data_dir + '*.cf32')

    # do the split
    for fp in filepaths:

        # get the data filename
        filename = fp.replace(data_dir, '')

        # the data file will be trainign data
        if split_counter < training_data * 10:
            #print(fp +  " --> " + data_dir + 'training/' + filename)
            shutil.move(fp, data_dir + 'training/' + filename)
        
        else: # the data file will be test data
            #print(fp +  " --> " + data_dir + 'test/' + filename)
            shutil.move(fp, data_dir + 'test/' + filename)


        # increment the split counter
        split_counter = split_counter + 1

        # reset the split counter
        if split_counter == 10:
            split_counter = 0


# consolidate data
def consolidate_data():

    # make expected directories
    os.system('mkdir -p data/training/k2')    
    os.system('mkdir -p data/test/k2')
    os.system('mkdir -p data/training/k3')
    os.system('mkdir -p data/test/k3')

    # consolidate data for k=2
    os.system('cp -f data/beacon/training/*.cf32 data/training/k2')
    os.system('cp -f data/nobeacon/training/*.cf32 data/training/k2')
    os.system('cp -f data/beacon/test/*.cf32 data/test/k2')
    os.system('cp -f data/nobeacon/test/*.cf32 data/test/k2')

    # consolidate data for k=3
    os.system('cp -f data/beacon/training/*.cf32 data/training/k3')
    os.system('cp -f data/nobeacon/training/*.cf32 data/training/k3')
    os.system('cp -f data/unknown/training/*.cf32 data/training/k3')
    os.system('cp -f data/beacon/test/*.cf32 data/test/k3')
    os.system('cp -f data/nobeacon/test/*.cf32 data/test/k3')
    os.system('cp -f data/unknown/test/*.cf32 data/test/k3')





# split the data
split_data('data/beacon/')
split_data('data/nobeacon/')
split_data('data/unknown/')

# consolidate data
consolidate_data()