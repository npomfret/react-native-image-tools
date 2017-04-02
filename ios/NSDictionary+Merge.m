#import "NSDictionary+Merge.h"

@implementation NSDictionary (Merge)

//thanks to: http://stackoverflow.com/questions/4011781/deep-combine-nsdictionaries

+ (NSDictionary *) dictionaryByMerging: (NSDictionary *) dict1 with: (NSDictionary *) dict2 {
    NSMutableDictionary * result = [NSMutableDictionary dictionaryWithDictionary:dict1];
    
    [dict2 enumerateKeysAndObjectsUsingBlock: ^(id key, id obj, BOOL *stop) {
        if (![dict1 objectForKey:key]) {
            [result setObject: obj forKey: key];
        } else if ([obj isKindOfClass:[NSDictionary class]]) {
            NSDictionary * newVal = [[dict1 objectForKey: key] dictionaryByMergingWith: (NSDictionary *) obj];
            [result setObject: newVal forKey: key];
        }
    }];
    
    return (NSDictionary *) [result mutableCopy];
}

- (NSDictionary *) dictionaryByMergingWith: (NSDictionary *) dict {
    return [[self class] dictionaryByMerging: self with: dict];
}

@end
