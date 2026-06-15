// ------------------------------------------------------------------------------
// <copyright file="ActionWithWorldPositionAttribute.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Actions
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Numerics;
    using System.Reflection;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Infrastructure.Loggers;

    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct)]
    public class ActionWithWorldPositionAttribute : Attribute
    {
        /// <summary>
        /// Mark an action as having a world position we can use for consideration weighting.
        /// </summary>
        /// <param name="fieldName">The field provided as a parameter will be used in these considerations.</param>
        public ActionWithWorldPositionAttribute(string fieldName)
        {
            this.FieldToUse = fieldName;
        }

        public string FieldToUse { get; set; }

        public Vector3 GetPosition(object classObject, ILogger logger)
        {
            Vector3? position = this.GetPropertyVector(classObject) ?? this.GetFieldVector(classObject);
            if (!position.HasValue)
            {
                logger.Error($"Action of type '{classObject.GetType()}' has a ActionWithWorldPosition attribute, but the marked field '{this.FieldToUse}' does not exist.");
            }

            return position ?? Vector3.Zero;
        }

        private Vector3? GetPropertyVector(object classObject)
        {
            var positionProperty = classObject.GetType().GetProperty(this.FieldToUse);
            if (positionProperty == null)
            {
                return null;
            }

            if (this.IsValidType(positionProperty.PropertyType))
            {
                return positionProperty.GetValue(classObject) as Vector3?;
            }

            return null;
        }

        private Vector3? GetFieldVector(object classObject)
        {
            var fieldProperty = classObject.GetType().GetField(this.FieldToUse);
            if (fieldProperty == null)
            {
                return null;
            }

            if (this.IsValidType(fieldProperty.FieldType))
            {
                return fieldProperty.GetValue(classObject) as Vector3?;
            }

            return null;
        }

        private bool IsValidType(Type type)
        {
            return type == typeof(Vector3) || type == typeof(Vector3?);
        }
    }
}
