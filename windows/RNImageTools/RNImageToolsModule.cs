using ReactNative.Bridge;
using System;
using System.Collections.Generic;
using Windows.ApplicationModel.Core;
using Windows.UI.Core;

namespace Com.Reactlibrary.RNImageTools
{
    /// <summary>
    /// A module that allows JS to share data.
    /// </summary>
    class RNImageToolsModule : NativeModuleBase
    {
        /// <summary>
        /// Instantiates the <see cref="RNImageToolsModule"/>.
        /// </summary>
        internal RNImageToolsModule()
        {

        }

        /// <summary>
        /// The name of the native module.
        /// </summary>
        public override string Name
        {
            get
            {
                return "RNImageTools";
            }
        }
    }
}
