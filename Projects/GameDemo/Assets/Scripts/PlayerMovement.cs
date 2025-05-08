using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace Braveheart
{
    public class PlayerMovement : GameObject
    {
        bool isGrounded = true;

        public float maxSpeed = 1.0f;
        public float MoveSpeed = 1.0f;

        private Vector2 shootDir;

        NetworkClientComponent client = null;
        float delay = 0.0f;
        Vector3 Pos;
        
        public PlayerMovement() : base()
        {
            
        }

        public void Start()
        {
            Debug.Log("Our ID: " + this.id.ToString());

            //NetworkClientComponent client = GetComponent<NetworkClientComponent>();


            //
            //Vector3 CamPos = cam.GetComponent<TransformComponent>().Translation;
            //cam.GetComponent<TransformComponent>().Translation = new Vector3(GetComponent<TransformComponent>().Translation.x, GetComponent<TransformComponent>().Translation.y, CamPos.z);
        }

        public void Update(float delta)
        {
            TransformComponent transform = GetComponent<TransformComponent>();
            Pos = transform.Translation;

            MovementUpdate();
            
            //ManageClient();
            if(delay >= 0.5f)
            {
                AttackInput();
                delay = 0.0f;
            }
            delay += delta;


        }

        public void AttackInput()
        {
            if (Input.IsMouseClicked(MouseCodes.Button0))
            {
                GetComponent<AudioSourceComponent>().PlayOneShot();

                Vector3 shot = Pos;
                Vector3 direction = new Vector3(shootDir.x, shootDir.y, 0); // Only affecting X-axis

                // Normalize direction to ensure consistent behavior with raycasts
                //direction = direction.Normalise();

                shot += direction * 5f;


                RayCastHit hit = Physics2D.RayCast(GetComponent<TransformComponent>().Translation, shot);

                if(hit.shapeId.Equals(id) == false && hit.count > 0)// != id)
                {
                    GameObject hitObj = new GameObject(hit.shapeId);
                    if (hitObj.GetComponent<Rigidbody2DComponent>() != null && hitObj.GetComponent<TagComponent>().Name == "Enemy")
                    {
                        shot.x /= 10;
                        hitObj.GetComponent<Rigidbody2DComponent>().ApplyLinearImpulseToCentre(new Vector3(shootDir.x, shootDir.y, 0));
                        hitObj.GetComponent<SpriteRendererComponent>().Colour = new Vector4(1.0f, 0.0f, 0.0f, 0.0f);
                        EnemyStats stats = hitObj.GetScript<EnemyStats>();
                        if (stats != null)
                        {
                            Debug.Log("HEALTH:" + stats.health.ToString());
                            stats.health -= 1;
                        }
                    }
                }

            }

            delay = 0.0f;
        }


        public void MovementUpdate()
        {
            CheckIsGrounded();

            Vector2 speed = GetComponent<Rigidbody2DComponent>().LinearVelocity;
            if (Math.Abs(speed.Magnitude()) <= maxSpeed)
            {
                if (Input.IsKeyDown(KeyCode.D))
                {

                    GetComponent<Rigidbody2DComponent>().ApplyLinearImpulseToCentre(new Vector3(0.5f * MoveSpeed, 0, 0), true);
                }
                if (Input.IsKeyDown(KeyCode.A))
                {

                    GetComponent<Rigidbody2DComponent>().ApplyLinearImpulseToCentre(new Vector3(-0.5f * MoveSpeed, 0, 0), true);
                }
                if (Input.IsKeyDown(KeyCode.W))
                {

                    GetComponent<Rigidbody2DComponent>().ApplyLinearImpulseToCentre(new Vector3(0, 0.5f * MoveSpeed, 0), true);
                }
                if (Input.IsKeyDown(KeyCode.S))
                {

                    Debug.Log("UP VECTOR: " + GetComponent<TransformComponent>().GetUp().ToString());
                    GetComponent<Rigidbody2DComponent>().ApplyLinearImpulseToCentre(new Vector3(0, -0.5f * MoveSpeed, 0), true);
                }
            }
            if(Math.Abs(speed.Magnitude()) > 0.1f)
            {
                shootDir = GetComponent<Rigidbody2DComponent>().LinearVelocity.Normalise();
            }
            if (Input.IsKeyDown(KeyCode.Space) && isGrounded)
            {
                GetComponent<Rigidbody2DComponent>().ApplyLinearImpulseToCentre(new Vector3(0, 0.5f, 0), true);
            }

        }

        void CheckIsGrounded()
        {
            Vector3 ground = new Vector3(GetComponent<TransformComponent>().Translation + new Vector3( 0, -1.5f, 0));
            //Physics2D.RayCast(GetComponent<TransformComponent>().Translation, ground);
            RayCastHit hitData = Physics2D.RayCast(GetComponent<TransformComponent>().Translation, ground);
            

            if (hitData.count == 0)
            {
                isGrounded = false;
            }
            else
            {
                isGrounded = true;
            }
        }

        public override void OnCollisionEnter(uint other)
        {
            //GameObject collider = new GameObject(other);
            //
            //Console.WriteLine("Player-specific collision");
            //if(collider.HasComponent<SpriteRendererComponent>())
            //    collider.GetComponent<SpriteRendererComponent>().Colour = new Vector4(1.0f, 0.0f, 0.0f, 0.0f);
        }





        void ManageClient()
        {
            if(client == null)
            {
                client = GetComponent<NetworkClientComponent>();
            }
            string latest = client.GetMessage();
            //Debug.LogWarning("LATEST MESSAGE " + latest);
        }
    }

}


