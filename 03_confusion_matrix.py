#!/usr/bin/env python3

import csv

metrics_json = {}

# read csv file with expected labeld
with open('data/test_dataset_expected.csv') as csvfile:

    # the reader
    reader = csv.reader(csvfile, delimiter=',')

    # the loop
    for row in reader:

        # the csv contains filename and beacon label
        data_filename = row[0]
        beacon_label = int(row[1])

        metrics_json[data_filename] = {
            'expected': beacon_label,
            'predicted': -1
        }


# read csv file with predicted labels
with open('data/test_dataset_predictions.csv') as csvfile:

    # the reader
    reader = csv.reader(csvfile, delimiter=',')

    # the loop
    for row in reader:

        # the csv contains filename and beacon label
        data_filename = row[0]
        beacon_label = int(row[1])

        metrics_json[data_filename]['predicted'] = beacon_label


# true negative (TN) counter
tn_counter = 0

# false negative (FN) counter
fn_counter = 0

# true postive (TP) counter
tp_counter = 0

# false positive (FP) counter
fp_counter = 0

for key in metrics_json:
    expected = metrics_json[key]['expected']
    predicted = metrics_json[key]['predicted']

    if predicted == 0 and expected == 0: # check for TN
        tn_counter = tn_counter + 1

    elif predicted == 0 and expected == 1: # check for FN
        fn_counter = fn_counter + 1

    elif predicted == 1 and expected == 1: # check for TP
        tp_counter = tp_counter + 1

    elif predicted == 1 and expected == 0: # check for FP
        fp_counter = fp_counter + 1

    else:
        print('this should not have happened')


print('\n          ACTUAL')
print('         1     0')
print('P     -------------')
print('R   1 | %3d | %3d |' % (tp_counter, fp_counter))
print('E     -------------')
print('D   0 | %3d | %3d |' % (fn_counter, tn_counter))
print('      -------------\n')


# calculate some metrics
sensitivity = tp_counter / (tp_counter + fn_counter)
specificity = tn_counter / (tn_counter + fp_counter)
balanced_accuracy = (sensitivity + specificity) / 2

print('Sensitivity = ' + str(sensitivity))
print('Specificity = ' + str(specificity))
print('Balanced Accuracy = ' + str(balanced_accuracy))
print('')