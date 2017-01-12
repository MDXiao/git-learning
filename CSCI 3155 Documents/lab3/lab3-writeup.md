#Lab 3 Write up 

##Problem 2a

Give one test case that behaves differently under dynamic scoping versus static scoping (and does not crash). Explain the test case and how they behave differently in your write-up.

Answer:

A test case that behaves differently under dynamic scoping and static scoping is the example that we did in class:

```
const x = 1;
const g = (y) => x;
const h = (x) => g(2);
h(3)
```

The result for h(3) if we were to use dynamic scoping would be 3, but the value for h(3) if we were to use dynamic scoping would be 1. This is because dynamic scoping in our eval function using mapping and changes based upon if it sees a similar variable in the expression. For example, x originally maps to 1 in the const x = 1 declaration, but then sees the h(3) and maps the value of x to 3. So it evaluates the rest of the expression based on the statement of x = 3 rather than x = 1. Static scoping just substitutes the const x = 1 to the rest of the expression immediately so data wouldn't be overwritten incorrectly, so with static scoping we get the correct value of 1 instead of 3.

##Problem 3d

Explain wheter the evaluation order is deterministic as specified by the judgment form e -> e' .

This is small step semantics, since due to this nature the expression e -> e' is deterministic. Since this format means that there is always at most one "next" step, and since there is a concrete method in stepping towards the end result, this means the judgment form e -> e' is deterministic. 


##Problem 4

Consider the small-step operational semantics for JAVASCRIPTY shown in Figures 7, 8, and 9. What is the evaluation order for e1 + e2? How do we change the rules to obtain the opposite evaluation order?

The evaluation order for e1 + e2 would be left to right because of the Search rules defined by the judgment forms. We first have to SearchBinary1, to step on e1. Since SearchBinaryArith2 needs for e1 to be a value, we can see that the evaluation order is left to right. We can change the rules of obtain the opposite evaluation order by first stepping on e2, then stepping on e1, then doing the arithmetic operation.



##Problem 5

5a). Give an example that illustrates the usefulness of short-circuit evaluation.

An example of a short-circuit evaluation is:

```
if(false && //Some long evaluation)

and

if(true || //Some long evaluation)

```

Using the && and || operators we can short-circuit as in not evaluate the second part of the function after only seeing the first part of the function. Like false && anything is always false, and true || anything is always true, and because of this we can short-circuit the expression by only returning based on the evaluation of the first part of the operator.

5b). Consider the small-step operational semantics for JAVASCRIPTY shown in Figures 7,8, and 9. Does e1 && e2 short circuit? Explain.

Yes, the expression e1 && e2 short circuits because there is no searchE1 in the search statements or search for any && or ||
