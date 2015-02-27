//
// MogKit
//
// Copyright (c) 2015 Mikael Hallendal. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <MogKit/MogReduce.h>


/**
 * A transformation takes a `MOGReducer` and returns a new `MOGReducer` with some transformation applied.
 *
 * @discussion a transformation can be stateful but the state is bound in the reducer created when the transformation
 * is applied to the output reducer. This means it's safe to use the same transformation to create new reducers.
 */
typedef MOGReducer *(^MOGTransformation) (MOGReducer *);

/**
 * A `MOGMapBlock` is a function you typically use with `MOGMap` which is a transformation of a single value
 * into a new value.
 */
typedef id (^MOGMapBlock) (id val);

/**
 * `MOGIndexedMapBlock` is the same as `MOGMapBlock`, only it also includes the index of the value in sequence
 * that is being processed.
 */
typedef id (^MOGIndexedMapBlock) (NSUInteger index, id val);

/**
 * `MOGPredicate` is a function that by examining the value returns YES or NO. It's typically used in `MOGFilter` or
 * `MOGKeep`.
 */
typedef BOOL (^MOGPredicate) (id val);

/**
 * Returns a transducer that doesn't transform the values.
 * This is useful as an input if you are reducing over a list of transducers.
 *
 * @return the identity transducer.
 */
MOGTransformation MOGIdentity(void);

/**
 * Creates a transducer that applies the `mapBlock` to transform each value passed through the transformation.
 *
 * @param mapBlock the transformation function.
 *
 * @return a transducer that applies the map transformation.
 */
MOGTransformation MOGMap(MOGMapBlock mapBlock);

/**
 * Creates a transducer that filters all values based on the `predicate` function. Values where `predicate` returns
 * NO are dropped.
 *
 * @param predicate the predicate function. Return YES to let the values through, NO to drop the value.
 *
 * @return a transducer that filters values keeps values where the predicate returns YES.
 */
MOGTransformation MOGFilter(MOGPredicate predicate);

/**
 * Creates a transducer that removes all values where the `predicate` function returns YES.
 * This is the reverse of `MOGFilter`.
 *
 * @param predicate the predicate function. Return YES to drop the values, NO to let them through.
 *
 * @return a transducer that remove values where predicate returns YES.
 */
MOGTransformation MOGRemove(MOGPredicate predicate);

/**
 * Creates a transducer that let `n` values through and then drops all remaining values passed through it.
 *
 * @param n the number of values to pass through.
 *
 * @return a stateful transducer that takes `n` values and then drops all remaining values.
 *
 * @see `MOGDrop`
 */
MOGTransformation MOGTake(NSUInteger n);

/**
 * Creates a transducer that pass values through while the `predicate` function returns YES. After the predicate
 * returns NO, all successive values are dropped.
 *
 * @param predicate the predicate function, return YES to let values through, NO to drop all remaining values.
 *
 * @return a stateful transducer that takes values until `predicate` returns NO and drops all remaining values.
 */
MOGTransformation MOGTakeWhile(MOGPredicate predicate);

/**
 * Creates a transducer that pass through every nth value and drops the other.
 *
 * @param n determines which values to pass through.
 *
 * @return a stateful transducer that returns every n values.
 */
MOGTransformation MOGTakeNth(NSUInteger n);

/**
 * Creates a transducer that drops the first `n` values and pass through all successive values.
 *
 * @param n number of values to drop.
 *
 * @return a stateful transducer that drops the n first values.
 *
 * @see `MOGTake`
 */
MOGTransformation MOGDrop(NSUInteger n);

/**
 * Creates a transducer that drops all values while the `predicate` function returns YES.
 * After that all values are passed through
 *
 * @param predicate the predicate function deciding whether to keep dropping values.
 *
 * @return a stateful transducer that drops all values until the predicate function returns YES.
 *
 * @see MOGTakeWhile
 */
MOGTransformation MOGDropWhile(MOGPredicate predicate);

/**
 * Creates a transducer that replaces values if they are found in the `replacements` dictionary. For each value it will
 * try to locate it as a key in the `replacements` dictionary, if found the corresponding value will be used instead,
 * otherwise the original value will be passed on unfiltered.
 *
 * @discussion This is the same as calling `MOGReplaceWithDefault` with `nil` as `defaultValue`.
 *
 * @param replacements a dictionary container values and replacements.
 *
 * @return a transducer that replaces all values found in `replacements`.
 *
 * @warning Keep in mind that if the replacements dictionary can't replace all values it should return the same type
 *          as the values passed in.
 */
MOGTransformation MOGReplace(NSDictionary *replacements);

/**
 * Creates a transducer that replaces values if they are found in the `replacements` dictionary. For each value it will
 * try to locate it as a key in the `replacements` dictionary, if found the corresponding value will be used instead,
 * otherwise the default value will be used. If the default value is also nil, original value will be passed on unfiltered.
 *
 * @discussion Calling this function with a `nil` as default value is the same as calling `MOGReplace`.
 *
 * @param replacements a dictionary container values and replacements.
 * @param defaultValue the default value to pass on if a replacement isn't found in the `replacements` dictionary.
 *
 * @return a transducer that replaces all values found in `replacements`.
 *
 * @warning Keep in mind that if the replacements dictionary can't replace all values it should return the same type
 *          as the values passed in.
 */
MOGTransformation MOGReplaceWithDefault(NSDictionary *replacements, id defaultValuee);

/**
 * Creates a transducer that keep values where `mapBlock` returns a non-nil value and drops all where nil is returned.
 *
 * @warning Keep in mind that the original value is passed on, not the value returned by `mapBlock`.
 *
 * @param mapBlock a function that determines if the transducer should pass on a value or not. non-nil to pass on,
 *        nil to drop it.
 *
 * @return a transducer that drops all values where `mapBlock` returns nil.
 *
 * @see `MOGFilter`, `MOGRemove` and `MOGKeepIndexed`.
 */
MOGTransformation MOGKeep(MOGMapBlock mapBlock);

/**
 * Creates a transducer a transducer that keeps values where `mapBlock` returns a non-nil value. This is similar to
 * `MOGKeep` with the difference that the index of the value is passed to `mapBlock` as well.
 *
 * @param mapBlock a function that determines if the transducer should pass on a value or not. non-nil to pass on,
 *        nil to drop it.
 *
 * @return a stateful transducer that drops all values where `mapBlock` returns nil.
 *
 * @see `MOGFilter`, `MOGRemove` and `MOGKeep`.
 */
MOGTransformation MOGKeepIndexed(MOGIndexedMapBlock mapBlock);

/**
 * Creates a transducer that drops all duplicates. Whether it's a duplicate is determined by `isEqual:`
 *
 * @return a stateful transducer that drops all duplicates.
 *
 * @see `MOGDedupe
 */
MOGTransformation MOGUnique(void);

/**
 * Creates a transducer that drops consecutive duplicated values.
 *
 * @discussion Drops consecutive duplicates, for a transducer that drops
 * all duplications, see `MOGUnique`.
 *
 * @return a stateful transducer that drops consecutive duplications.
 *
 * @see `MOGUniqueTransducer`
 */
MOGTransformation MOGDedupe(void);

/**
 * Creates a transducer that flattens values that conforms to `NSFastEnumeration`. Other values are passed
 * through untouched.
 *
 * @return a transducer that concat the values.
 */
MOGTransformation MOGCat(void);

/**
 * Creates a transducer that first apply the mapBlock on all values and then concatenates the results. It's the
 * composition of `MogMap` . `MogCat`.
 *
 * @param mapBlock the transformation function.
 *
 * @return a transducer that applies `mapBlock` on all values and then concatenates the result.
 */
MOGTransformation MOGMapCat(MOGMapBlock mapBlock);

/**
 * Creates a transducer that splits the the values into separate `NSArray`s every time the `partitioningBlock`
 * returns a new value. The final partition will be sent on complete.
 *
 * @param partitioningBlock the return value of calling the block decides on whether a new partition should be created.
 *
 * @return a stateful transducer that splits incoming values into separate partitions.
 */
MOGTransformation MOGPartitionBy(MOGMapBlock partitioningBlock);

/**
 * Creates a transducer that splits the values into separate `NSArray`s every `size` elements. A smaller array may be
 * sent at the end if there are less values than `size` accumulated in the transducer at complete.
 *
 * @param size the partition size
 *
 * @return a stateful transducer that splits incoming values into separate partitions of size `size`.
 */
MOGTransformation MOGPartition(NSUInteger size);

/**
 * Creates a transducer that creates a window of size `length` by examining the values passed through.
 * The window will always contain the last `length` values passed through. Until `length` values have passed through
 * the window will contain the first value in all slots. Each value passed through is replaced by an array with the
 * current window content.
 *
 * @param length the length of the window.
 *
 * @return a stateful transducer that replaces each value with an array containing the current window content.
 */
MOGTransformation MOGWindow(NSUInteger length);

/**
 * Creates the composite transducer by applying `f` to `g`.
 *
 * @discussion Note that transducers are composed right to left but the resulting transformation runs left to right.
 *             In this case, it means that the transformation done by `f` will be applied before that of `g`.
 *
 * @param f the first transducer
 * @param g the second transducer
 *
 * @return a composite transducer of applied `f` to `g`.
 */
MOGTransformation MOGCompose(MOGTransformation f, MOGTransformation g);

/**
 * Creates a transducer which is the composition of `transducers`.
 *
 * @discussion Note that transducers are composed right to left but the resulting transformation runs left to right.
 *
 * @param transducers an `NSArray` of `MOGTransformation`s.
 *
 * @return a composite transducer of all transducers in `transducers`.
 *
 * @see `MOGCompose`
 */
MOGTransformation MOGComposeArray(NSArray *transducers);

/**
 * Applied the transformation to `source`. This is the step when input, transformation and output are combined
 * to transform the source values into output values based on the `reducer` and the `initial` value.
 *
 * @param source any class conforming to the `NSFastEnumeration` protocol.
 * @param reducer the reducer to collect the transformed values into the result of this function.
 * @param transducer the transformation to apply.
 *
 * @return the final value collected by `reducer`.
 */
id MOGTransform(id<NSFastEnumeration> source, MOGReducer *reducer, MOGTransformation transformation);

/**
 * Applied the transformation to `source`. This is the step when input, transformation and output are combined
 * to transform the source values into output values based on the `reducer` and the `initial` value.
 *
 * @param source any class conforming to the `NSFastEnumeration` protocol.
 * @param reducer the reducer to collect the transformed values into the result of this function.
 * @param initial the initial value to pass as the accumulator to `reducer`.
 * @param transducer the transformation to apply.
 *
 * @return the final value collected by `reducer`.
 */
id MOGTransformWithInitial(id<NSFastEnumeration> source, MOGReducer *reducer, id initial, MOGTransformation transformation);
