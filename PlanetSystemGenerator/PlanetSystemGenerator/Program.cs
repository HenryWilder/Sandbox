using System;
using System.Numerics;
using Raylib_cs;
using static Raylib_cs.Raylib;

namespace PlanetSystemGenerator
{
    /// <summary>Data for a single ring.</summary>
    struct Ring
    {
        /// <summary>Inner radius.</summary>
        public float iRad;
        /// <summary>Outer radius.</summary>
        public float oRad;
        /// <summary>Color of ring when rendering</summary>
        public Color color;
    }

    /// <summary>Body whose position is static and unchanging.</summary>
    class AnchorBody
    {
        /// <summary>Explicitly generates the body.</summary>
        public AnchorBody(float radius, Ring[] rings)
        {
            Radius = radius;
            Rings = rings;
        }

        /// <summary>Size of body when rendering.</summary>
        public float Radius { get; init; }

        /// <summary>Data for optional ring system.</summary>
        public Ring[] Rings { get; init; }
    }

    /// <summary>Body whose position is calculated relative to the anchor.</summary>
    class MajorBody : AnchorBody
    {
        /// <summary>Explicitly generates the body.</summary>
        public MajorBody(float radius, Ring[] rings, float orbit, float period, float startingT)
            : base(radius, rings)
        {
            Orbit = orbit;
            Period = period;
            T = startingT;
            position = new Vector3(0);
            UpdatePosition();
        }

        /// <summary>Size of orbit.</summary>
        public float Orbit { get; init; }

        /// <summary>Length of a full orbit.</summary>
        public float Period { get; init; }

        /// <summary>"Time" along orbit path.</summary>
        public float T { get; set; }

        /// <summary>Storage of this frame's position. (to minimize redundant calculation)</summary>
        public Vector3 Position { get { return position; } } 
        Vector3 position;

        /// <summary>Call this only once per tick.</summary>
        public void UpdatePosition()
        {
            const float pi2 = 2.0f * MathF.PI;
            float _t = pi2 * T / Period;
            position.X = MathF.Sin(_t) * Orbit;
            position.Y = MathF.Cos(_t) * Orbit;
            position.Z = 0.0f;
        }
    }

    /// <summary>A body whose position is dependent on that of a parent.</summary>
    class MinorBody : MajorBody
    {
        /// <summary>Explicitly generates the body.</summary>
        public MinorBody(float radius, Ring[] rings, float orbit, float period, float startingT, MajorBody parent)
            : base(radius, rings, orbit, period, startingT)
        {
            Parent = parent;
        }

        /// <summary>Reference to the parent body whose position this body orbits.</summary>
        public MajorBody Parent { get; init; }
    }

    public class Program
    {
        readonly static Random rnd = new();

        /// <summary>Returns a random floating-point precision number that is within the specified range.</summary>
        public static float RandFlt(float min = 0.0f, float max = 1.0f)
        {
            return (float)rnd.NextDouble() * (max - min) + min;
        }

#if false // Set to true if this ends up being helpful
        public static Vector3 RandPointInSphere(Vector3 center, Vector3 radii)
        {
            Vector3 offset = new Vector3(
                RandFlt(-1.0f, 1.0f),
                RandFlt(-1.0f, 1.0f),
                RandFlt(-1.0f, 1.0f));
            return center + Vector3.Normalize(offset) * radii;
        }
#endif

        public static void Main()
        {
            AnchorBody anchor = new(radius: RandFlt(50, 70), rings: new Ring[1]); // Star
            MajorBody[] major; // Planets
            MinorBody[] minor; // Moons

            major = new MajorBody[rnd.Next(3, 25)];
            for (int i = 0; i < major.Length; ++i)
            {
                major[i] = new(
                    radius: RandFlt(10, 20),
                    rings: new Ring[rnd.Next(6)],
                    orbit: anchor.Radius + RandFlt(20, 200),
                    period: RandFlt(0.1f, 30),
                    startingT: RandFlt(0.0f, 1.0f)); // 0...1
            }
        }
    }
}
