                                                                                                                                                                                                                                                                                                                                                              using System;
using System.Numerics;
using Raylib_cs;
using static Raylib_cs.Raylib;
using static Raylib_cs.Raymath;

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
            if (Rings.Length > 0)
            {
                RingTex = LoadRenderTexture(256, 256);
                BeginTextureMode(RingTex);
                {
                    Vector2 center = new Vector2(RingTex.texture.width, RingTex.texture.height) / 2;
                    foreach (Ring ring in Rings)
                    {
                        DrawRing(
                            center: center,
                            innerRadius: ring.iRad,
                            outerRadius: ring.oRad,
                            startAngle: 0,
                            endAngle: 360,
                            segments: 64,
                            color: ring.color);
                    }
                }
                EndTextureMode();
            }
        }
        ~AnchorBody()
        {
            if (Rings.Length > 0)
                UnloadRenderTexture(RingTex);
        }

        /// <summary>Size of body when rendering.</summary>
        public float Radius { get; init; }

        /// <summary>Color of body when rendering.</summary>
        public Color Color { get; init; }

        /// <summary>Data for optional ring system.</summary>
        public Ring[] Rings { get; init; }
        public RenderTexture2D RingTex { get; init; }
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
            base.Step(dt / (MathF.Sqrt(Parent.Radius) * 2));
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
        
        /// <summary>
        /// Generates a random point in a 3D ellipsoid (with X, Y, and Z widths defined by <paramref name="radii"/>) around the <paramref name="center"/>.
        /// </summary>
        /// <param name="center">The point around which the return will be randomly generated (within specified <paramref name="radii"/>).</param>
        /// <param name="radii">The 3D radii around <paramref name="center"/> inside of which the point will be generated.</param>
        /// <returns>A Numerics.Vector3 in a 3D ellipsoid around a specified point.</returns>
        public static Vector3 RandPointInSphere(Vector3 center, Vector3 radii)
        {
            Vector3 offset = new Vector3(
                RandFlt(-1.0f, 1.0f),
                RandFlt(-1.0f, 1.0f),
                RandFlt(-1.0f, 1.0f));
            return center + Vector3.Normalize(offset) * radii;
        }


        /// <summary>
        /// Returns a color based on blackbody radiation with a temperature (inclusively) between <paramref name="minTemp"/> and <paramref name="maxTemp"/>.
        /// </summary>
        /// <param name="minTemp">The (inclusive) minimum temperature that can be generated randomly, measured in Kelvin.</param>
        /// <param name="maxTemp">The (inclusive) maximum temperature that can be generated randomly, measured in Kelvin.</param>
        /// <returns>An instance of Raylib_cs.Color, on the blackbody scale.</returns>
        public static Color RandColor_Blackbody(float minTemp = 500, float maxTemp = 12000)
        {
            float temperature = RandFlt(minTemp, maxTemp);
            float amount = (temperature - 500) / (12000 - 500);
            Color color;
            // Uses cubic bezier curves to calculate blackbody color.
            // I completely made up these numbers btw; they are not scientifically accurate.
            color.r = (byte)Lerp(Lerp(255, 255, amount), Lerp(255, 175, amount), amount);
            color.g = (byte)Lerp(Lerp(55, 250, amount), Lerp(250, 200, amount), amount);
            color.b = (byte)Lerp(Lerp(0, 200, amount), Lerp(200, 255, amount), amount);
            color.a = 255;
            return color;
        }
        /// <summary>
        /// Generates a completely random color with full opacity.
        /// </summary>
        /// <returns>An instance of Raylib_cs.Color with an alpha of 255.</returns>
        public static Color RandColor_Solid()
        {
            return new(rnd.Next(256), rnd.Next(256), rnd.Next(256), a: 255);
        }
        /// <summary>
        /// Generates a completely random color with random opacity.
        /// </summary>
        /// <returns>An instance of Raylib_cs.Color.</returns>
        public static Color RandColor_Transparent()
        {
            return new(rnd.Next(256), rnd.Next(256), rnd.Next(256), rnd.Next(256));
        }

        public static void Main()
        {
            InitWindow(1280, 720, "Planet System Generator");
            SetTargetFPS(60);

            float anchorRadius = RandFlt(35, 100);
            int anchorRingCount = rnd.Next(3);
            Ring[] anchorRingSystem = new Ring[anchorRingCount];
            for (int i = 0; i < anchorRingCount; ++i)
            {
                anchorRingSystem[i].iRad = anchorRadius + RandFlt(50, 400);
                anchorRingSystem[i].oRad = anchorRingSystem[i].iRad + RandFlt(10, 100);
                anchorRingSystem[i].color = RandColor_Transparent();
            }
            float anchorAvgTemperature = (anchorRadius - 35) / 100;

            // Star
            AnchorBody anchor = new(
                radius: anchorRadius,
                color: RandColor_Blackbody(Lerp(500, 10000, anchorAvgTemperature), Lerp(700, 12000, anchorAvgTemperature)),
                rings: anchorRingSystem);
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
                        color: RandColor_Solid(),
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
                    MajorBody parentBody = major[rnd.Next(major.Length)];
                    float bodyRadius = RandFlt(2, 6);
                    float orbitRadius = parentBody.Radius + bodyRadius + RandFlt(0.1f, 3) * parentBody.Radius;
                    minor[i] = new(
                        radius: bodyRadius,
                        color: RandColor_Solid(),
                        rings: new Ring[rnd.Next(7)],
                        orbit: orbitRadius,
                        period: MathF.Pow(orbitRadius, 2) / 5000,
                        startingT: RandFlt(0.0f, 1.0f),
                        parent: parentBody);
                }
            }

            // !! We are using the Z axis for UP !! 

            Camera3D camera;
            camera.position = Vector3.UnitZ * 100;
            camera.target = Vector3.Zero;
            camera.up = Vector3.UnitY;
            camera.fovy = 70;
            camera.projection = CameraProjection.CAMERA_PERSPECTIVE;

            SetCameraMode(camera, CameraMode.CAMERA_ORBITAL);

            while (!WindowShouldClose())
            {
                float dt = GetFrameTime();
                foreach (MajorBody body in major)
                {
                    body.Step(dt);
                }
                foreach (MajorBody body in minor)
                {
                    body.Step(dt);
                }

                UpdateCamera(ref camera);

                BeginDrawing();
                {
                    ClearBackground(Color.BLACK);

                    BeginMode3D(camera);
                    {
                        DrawSphere(
                            centerPos: Vector3.Zero,
                            radius: anchor.Radius,
                            color: anchor.Color);

                        foreach (MajorBody body in major)
                        {
                            DrawCircle3D(
                                center: Vector3.Zero,
                                radius: body.Orbit,
                                rotationAxis: Vector3.UnitZ,
                                rotationAngle: 0.0f,
                                color: Color.BROWN);
                            DrawSphere(
                                centerPos: body.Position,
                                radius: body.Radius,
                                color: body.Color);
                        }
                        foreach (MinorBody body in minor)
                        {
                            DrawCircle3D(
                                center: body.Parent.Position,
                                radius: body.Orbit,
                                rotationAxis: Vector3.UnitZ,
                                rotationAngle: 0.0f,
                                color: Color.DARKGRAY);
                            DrawSphere(
                                centerPos: body.Position,
                                radius: body.Radius,
                                color: body.Color);
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              