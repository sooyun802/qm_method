## Quine-McCluskey Method

```flow
st=>start: Start
op1=>operation: Compare the Items of 2 Different Groups in a Column
cond1=>condition: Difference is 1-bit
op2=>operation: Check Both Items and Add to Next Column
cond2=>condition: All Items Compared
op3=>operation: Add Unchecked Items to Prime Implicants
cond3=>condition: All Columns Compared
op4=>operation: Prime Implicant Chart Completed
op5=>operation: Remove Items that Overlap with Essential Prime Implicants from the Prime Implicant Chart
cond4=>condition: Items Left in the Prime Implicant Chart
op6=>operation: Gradually Remove Items from the Prime Implicant Chart
e=>end

st->op1->cond1
cond1(yes)->op2
cond1(no)->cond2
cond2(no)->op1
cond2(yes)->op3
op3->cond3
cond3(no)->op1
cond3(yes)->op4->op5->cond4
cond4(yes)->op6->cond4
cond4(no)->e
```
