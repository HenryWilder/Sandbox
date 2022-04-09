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
        public AnchorBody(float radius, Color color, Ring[] rings)
        {
            Radius = radius;
            Color = color;
            Rings = rings;
        }

        /// <summary>Size of body when rendering.</summary>
        public float Radius { get; init; }

        /// <summary>Color of body when rendering.</summary>
        public Color Color { get; init; }

        /// <summary>Data for optional ring system.</summary>
        public Ring[] Rings { get; init; }
    }

    /// <summary>Body whose position is calculated relative to the anchor.</summary>
    class MajorBody : AnchorBody
    {
        /// <summary>Explicitly generates the body.</summary>
        public MajorBody(float radius, Color color, Ring[] rings, float orbit, float period, float startingT)
            : base(radius, color, rings)
        {
            Orbit = orbit;
            Period = period;
            t = startingT;
            position = Vector3.Zero;
        }

        /// <summary>Size of orbit.</summary>
        public float Orbit { get; init; }

        /// <summary>Length of a full orbit.</summary>
        public float Period { get; init; }

        /// <summary>"Time" along orbit path.</summary>
        public float T { get { return t; } }
        protected float t;

        /// <summary>Storage of this frame's position. (to minimize redundant calculation)</summary>
        public Vector3 Position { get { return position; } } 
        protected Vector3 position;

        /// <summary>Call this only once per tick.</summary>
        public virtual void Step(float dt)
        {
            t += dt / Period;
            const float pi2 = MathF.PI * 2;
            float radianT = pi2 * t;
            position.X = MathF.Sin(radianT) * Orbit;
            position.Y = MathF.Cos(radianT) * Orbit;
            position.Z = 0.0f;
        }        
    }

    /// <summary>A body whose position is dependent on that of a parent.</summary>
    class MinorBody : MajorBody
    {
        /// <summary>Explicitly generates the body.</summary>
        public MinorBody(float radius, Color color, Ring[] rings, float orbit, float period, float startingT, MajorBody parent)
            : base(radius, color, rings, orbit, period, startingT)
        {
            Parent = parent;
        }

        /// <summary>Reference to the parent body whose position this body orbits.</summary>
        public MajorBody Parent { get; init; }

        /// <summary>Call this only once per tick.</summary>
        public override void Step(float dt)
        {
            base.Step(dt);
            position += Parent.Position;
        }
    }

    public class Program
    {
        readonly static Random rnd = new();

        /// <summary>Returns a random floating-point precision number that is within the specified range.</summary>
        public static float RandFlt(float min = 0.0f, float max = 1.0f)
        {
            return (float)rnd.NextDouble() * (max - min) + min;
        }
        
        public static Vector3 RandPointInSphere(Vector3 center, Vector3 radii)
        {
            Vector3 offset = new Vector3(
                RandFlt(-1.0f, 1.0f),
                RandFlt(-1.0f, 1.0f),
                RandFlt(-1.0f, 1.0f));
            return center + Vector3.Normalize(offset) * radii;
        }

        public static Color RandColor()
        {
            return new(rnd.Next(256), rnd.Next(256), rnd.Next(256), a: 255);
        }

        public static void Main()
        {
            // Star
            AnchorBody anchor = new(
                radius: RandFlt(35, 100),
                color: RandColor(),
                rings: new Ring[rnd.Next(3)]);
            MajorBody[] major; // Planets
            MinorBody[] minor; // Moons

            {
                major = new MajorBody[rnd.Next(3, 25)];
                float lastOrbit = anchor.Radius + RandFlt(50, 200);
                for (int i = 0; i < major.Length; ++i)
                {
                    float bodyRadius = RandFlt(10, 20);
                    float orbitRadius = lastOrbit + RandFlt(10, 80) + bodyRadius;
                    lastOrbit = orbitRadius + bodyRadius;
                    major[i] = new(
                        radius: bodyRadius,
                        color: RandColor(),
                        rings: new Ring[rnd.Next(7)],
                        orbit: orbitRadius,
                        period: MathF.Pow(orbitRadius, 2) / 5000,
                        startingT: RandFlt(0.0f, 1.0f));
                }
            }

            {
                minor = new MinorBody[rnd.Next(3, 25)];
                for (int i = 0; i < minor.Length; ++i)
                {
                    float bodyRadius = RandFlt(2, 10);
                    MajorBody parentBody = major[rnd.Next(major.Length)];
                    minor[i] = new(
                        radius: bodyRadius,
                        color: RandColor(),
                        rings: new Ring[rnd.Next(7)],
                        orbit: orbitRadius,
                        period: MathF.Pow(orbitRadius, 2) / 5000,
                        startingT: RandFlt(0.0f, 1.0f),
                        parent: parent);
                }
            }

            InitWindow(1280,720,"Planet System Generator");
            SetTargetFPS(60);
            SetExitKey(0); // Don't close when esc is pressed

            // !! We are using the Z axis for UP !! 

            Camera3D camera;
            camera.position = Vector3.UnitZ * 1000;
            camera.target = Vector3.Zero;
            camera.up = Vector3.UnitY;
            camera.fovy = 70;
            camera.projection = CameraProjection.CAMERA_PERSPECTIVE;

            while (!WindowShouldClose())
            {
                float dt = GetFrameTime();
                foreach (MajorBody body in major)
                {
                    body.Step(dt);
                }

                BeginDrawing();
                {
                    ClearBackground(Color.BLACK);

                    BeginMode3D(camera);
                    {
                        DrawSphere(
                            centerPos: Vector3.Zero,
                            radius: anchor.Radius,
                            color: Color.YELLOW);

                        foreach (MajorBody body in major)
                        {
                            DrawSphere(
                                centerPos: body.Position,
                                radius: body.Radius,
                                color: Color.ORANGE);
                        }
                    }
                    EndMode3D();
                }
                EndDrawing();
            }

            CloseWindow();
        }
    }
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              